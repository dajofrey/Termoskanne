// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Topbar.h"
#include "Program.h"
#include "Session.h"

#include "../Common/Macros.h"

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

// FUNCTIONS =======================================================================================

TK_API_RESULT tk_core_handleTopbarInput(
    tk_core_Tile *Tile_p, nh_api_WSIEvent Event)
{
    tk_core_MacroTile *MacroTile_p = Tile_p->p;
    tk_core_MacroTab *MacroTab_p = MacroTile_p->MacroTabs.pp[MacroTile_p->current];

    switch (Event.type) {
        case NH_API_WSI_EVENT_MOUSE :
//            tk_core_handleTopbarMouseInput(Tile_p, Event.Mouse);
            break;
    }

    return TK_API_SUCCESS;
}

static tk_core_Glyph tk_core_getGlyphHelper(
    NH_API_UTF32 codepoint)
{
    tk_core_Glyph Glyph;
    memset(&Glyph, 0, sizeof(tk_core_Glyph));
    Glyph.codepoint = codepoint;
    Glyph.mark |= TK_CORE_MARK_LINE_HORIZONTAL;
    Glyph.mark |= TK_CORE_MARK_ACCENT;
    return Glyph;
}

TK_API_RESULT tk_core_drawTopbarRow(
    tk_core_Tile *Tile_p, tk_core_Glyph *Glyphs_p, int cols, int row, bool standardIO)
{
    tk_core_Session *Session_p = nh_core_getWorkloadArg();
    tk_core_Topbar *Topbar_p = &TK_CORE_MACRO_TAB(Tile_p)->Topbar;
 
    for (int i = 0; i < cols; ++i) {
        Glyphs_p[i] = tk_core_getGlyphHelper(' ');
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
            Glyphs_p[i] = tk_core_getGlyphHelper('q');
            Glyphs_p[i].mark |= TK_CORE_MARK_LINE_GRAPHICS;
        }
    }

    for (int i = 0; i < cols; ++i) {
        Glyphs_p[i].Attributes.bold = true;
        Glyphs_p[i].codepoint = 0;
        Glyphs_p[i].mark |= TK_CORE_MARK_ACCENT_BACKGROUND_2;
        Glyphs_p[i].Background.custom = true;
    }
 

    // TODO

    return TK_API_SUCCESS;
}
