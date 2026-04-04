// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Topbar.h"

#include "../Common/Macros.h"

#include "../../tk-core/Core/Session.h"
#include "../../tk-gfx/Renderer/Renderer.h"

#include "nh-core/System/Memory.h"
#include "nh-core/System/Thread.h"
#include "nh-core/Util/Time.h"

#include "nh-encoding/Encodings/UTF32.h"
#include "nh-encoding/Encodings/UTF8.h"

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

// FUNCTIONS =========================================================================================

tk_terminal_Topbar tk_terminal_initTopbar() {
    tk_terminal_Topbar Topbar;
    memset(&Topbar, 0, sizeof(tk_terminal_Topbar));
    return Topbar;
}

static TK_API_RESULT tk_terminal_handleTopbarMouseInput(
    tk_core_Tile *Tile_p, nh_api_MouseEvent Event)
{
    if (Event.trigger == NH_API_TRIGGER_PRESS && Event.Position.x-1 < 9 && Event.Position.x > 0) {
        ((tk_core_MacroTile*)Tile_p->p)->current = Event.Position.x-1;
    }
//    if (Event.trigger == NH_API_TRIGGER_PRESS && Event.Position.x == TK_CORE_MACRO_TAB(Tile_p)->Topbar.quitPosition) {
//        TK_CORE_MICRO_TAB(TK_CORE_MACRO_TAB(Tile_p))->Tile_p->close = true;
//    }
    if (Event.trigger == NH_API_TRIGGER_MOVE && (Event.Position.x >= TK_CORE_MACRO_TAB(Tile_p)->Topbar.quitPosition-1 && Event.Position.x <= TK_CORE_MACRO_TAB(Tile_p)->Topbar.quitPosition+1)) {
        TK_CORE_MACRO_TAB(Tile_p)->Topbar.quitHover = true;
    }
    if (Event.trigger == NH_API_TRIGGER_MOVE && (Event.Position.x < TK_CORE_MACRO_TAB(Tile_p)->Topbar.quitPosition-1 || Event.Position.x > TK_CORE_MACRO_TAB(Tile_p)->Topbar.quitPosition+1)) {
        TK_CORE_MACRO_TAB(Tile_p)->Topbar.quitHover = false;
    }
    if (Event.trigger == NH_API_TRIGGER_MOVE && (Event.Position.x >= TK_CORE_MACRO_TAB(Tile_p)->Topbar.tilingPosition-1 && Event.Position.x <= TK_CORE_MACRO_TAB(Tile_p)->Topbar.tilingPosition+1)) {
        TK_CORE_MACRO_TAB(Tile_p)->Topbar.tilingHover = true;
    }
    if (Event.trigger == NH_API_TRIGGER_MOVE && (Event.Position.x < TK_CORE_MACRO_TAB(Tile_p)->Topbar.tilingPosition-1 || Event.Position.x > TK_CORE_MACRO_TAB(Tile_p)->Topbar.tilingPosition+1)) {
        TK_CORE_MACRO_TAB(Tile_p)->Topbar.tilingHover = false;
    }
    return TK_API_SUCCESS;
}

TK_API_RESULT tk_terminal_handleTopbarInput(
    tk_core_Tile *Tile_p, nh_api_WSIEvent Event)
{
    tk_core_MacroTile *MacroTile_p = Tile_p->p;
    tk_core_MacroTab *MacroTab_p = MacroTile_p->MacroTabs.pp[MacroTile_p->current];

    switch (Event.type) {
        case NH_API_WSI_EVENT_MOUSE :
            tk_terminal_handleTopbarMouseInput(Tile_p, Event.Mouse);
            break;
    }

    return TK_API_SUCCESS;
}

static tk_core_Glyph tk_terminal_getGlyphHelper(
    NH_API_UTF32 codepoint)
{
    tk_core_Glyph Glyph;
    memset(&Glyph, 0, sizeof(tk_core_Glyph));
    Glyph.codepoint = codepoint;
    Glyph.mark |= TK_CORE_MARK_LINE_HORIZONTAL;
    Glyph.mark |= TK_CORE_MARK_ACCENT;
    return Glyph;
}

TK_API_RESULT tk_terminal_drawTopbar(
    tk_core_Tile *Tile_p, tk_core_Glyph *Glyphs_p, int cols, int row, bool standardIO)
{
    tk_core_Session *Session_p = nh_core_getWorkloadArg();
    tk_core_Topbar *Topbar_p = &TK_CORE_MACRO_TAB(Tile_p)->Topbar;
 
    for (int i = 0; i < cols; ++i) {
        Glyphs_p[i] = tk_terminal_getGlyphHelper(' ');
        if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.highContrast) {
            Glyphs_p[i].Attributes.reverse = true;
        }
        if (!Tile_p && !standardIO) {
            Glyphs_p[i].codepoint = 'q';
            Glyphs_p[i].mark |= TK_CORE_MARK_LINE_GRAPHICS;
        }
    }

    if (!Tile_p) {return TK_API_SUCCESS;}

    for (int i = 0; i < cols && !(Session_p->Config.Topbar.on); ++i) {
        if (Glyphs_p[i].codepoint == ' ' || Glyphs_p[i].codepoint == 0) {
            Glyphs_p[i] = tk_terminal_getGlyphHelper('q');
            Glyphs_p[i].mark |= TK_CORE_MARK_LINE_GRAPHICS;
        }
    }

    for (int i = 0; i < cols; ++i) {
        Glyphs_p[i].Attributes.bold = true;
        Glyphs_p[i].codepoint = 0;
        Glyphs_p[i].mark |= TK_CORE_MARK_ACCENT_BACKGROUND_2;
        Glyphs_p[i].Background.custom = true;
    }
 
    int count = 2;
    for (int i = 0; Tile_p && i < TK_CORE_MACRO_TILE(Tile_p)->MacroTabs.size; ++i) {
        tk_core_MacroTab *Tab_p = (tk_core_MacroTab*)TK_CORE_MACRO_TILE(Tile_p)->MacroTabs.pp[i];
        if (Tab_p->active) {
            Glyphs_p[count].codepoint = i + '1';

            if (TK_CORE_MACRO_TILE(Tile_p)->current == i) {

                Glyphs_p[count - 2].Attributes.reverse = false;
                Glyphs_p[count - 1].Attributes.reverse = false;
                Glyphs_p[count - 0].Attributes.reverse = false;
                Glyphs_p[count + 1].Attributes.reverse = false;
                Glyphs_p[count + 2].Attributes.reverse = false;

                Glyphs_p[count - 2].codepoint = 0;
                Glyphs_p[count - 2].mark |= TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_ACCENT_BACKGROUND;
                Glyphs_p[count - 2].Background.custom = true;

                Glyphs_p[count - 1].codepoint = 0;
                Glyphs_p[count - 1].mark |= TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_ACCENT_BACKGROUND;
                Glyphs_p[count - 1].Background.custom = true;

                Glyphs_p[count].Background.custom = true;
                Glyphs_p[count].mark = TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND;

                Glyphs_p[count + 1].Background.custom = true;
                Glyphs_p[count + 1].mark = TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND;
                Glyphs_p[count + 1].codepoint = 0;

                Glyphs_p[count + 2].Background.custom = true;
                Glyphs_p[count + 2].mark = TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND;
                Glyphs_p[count + 2].codepoint = 0;

            } else {

                Glyphs_p[count - 2].codepoint = 0;
                Glyphs_p[count - 2].mark |= TK_CORE_MARK_ACCENT_BACKGROUND_2;
                Glyphs_p[count - 2].Background.custom = true;

                Glyphs_p[count - 1].codepoint = 0;
                Glyphs_p[count - 1].mark |= TK_CORE_MARK_ACCENT_BACKGROUND_2;
                Glyphs_p[count - 1].Background.custom = true;

                Glyphs_p[count].Background.custom = true;
                Glyphs_p[count].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;

                Glyphs_p[count + 1].Background.custom = true;
                Glyphs_p[count + 1].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
                Glyphs_p[count + 1].codepoint = 0;

                Glyphs_p[count + 2].Background.custom = true;
                Glyphs_p[count + 2].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
                Glyphs_p[count + 2].codepoint = 0;
 
            }

            count += 5;
        }
    }

    Glyphs_p[count - 2].codepoint = 0;
    Glyphs_p[count - 2].mark |= TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Glyphs_p[count - 2].Background.custom = true;

    Glyphs_p[count - 1].codepoint = 0;
    Glyphs_p[count - 1].mark |= TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Glyphs_p[count - 1].Background.custom = true;

    Glyphs_p[count].Background.custom = true;
    Glyphs_p[count].codepoint = '+';
    Glyphs_p[count].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;

    Glyphs_p[count + 1].Background.custom = true;
    Glyphs_p[count + 1].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Glyphs_p[count + 1].codepoint = 0;

    Glyphs_p[count + 2].Background.custom = true;
    Glyphs_p[count + 2].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Glyphs_p[count + 2].codepoint = 0;

    count = cols-2;
    Topbar_p->quitPosition = count;
    TK_CORE_MARK_E background = Topbar_p->quitHover ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    Glyphs_p[count - 1].codepoint = 0;
    Glyphs_p[count - 1].mark |= background;
    Glyphs_p[count - 1].Background.custom = true;

    Glyphs_p[count].Background.custom = true;
 
    Glyphs_p[count].codepoint = 0x00d7;
    Glyphs_p[count].mark = TK_CORE_MARK_ACCENT | background;

    Glyphs_p[count + 1].Background.custom = true;
    Glyphs_p[count + 1].mark = TK_CORE_MARK_ACCENT | background;
    Glyphs_p[count + 1].codepoint = 0;

    count = cols - 5;
    Topbar_p->tilingPosition = count;
    background = Topbar_p->tilingHover ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    Glyphs_p[count - 1].codepoint = 0;
    Glyphs_p[count - 1].mark |= background;
    Glyphs_p[count - 1].Background.custom = true;

    Glyphs_p[count].Background.custom = true;
    Glyphs_p[count].Attributes.bold = false;
    Glyphs_p[count].codepoint = 0x2237;
    Glyphs_p[count].mark = TK_CORE_MARK_ACCENT | background;

    Glyphs_p[count + 1].Background.custom = true;
    Glyphs_p[count + 1].mark = TK_CORE_MARK_ACCENT | background;
    Glyphs_p[count + 1].codepoint = 0;

    return TK_API_SUCCESS;
}
