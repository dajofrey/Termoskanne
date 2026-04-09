// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Bottombar.h"
#include "Program.h"
#include "Session.h"

#include "../Common/Macros.h"

#include "../../tk-gfx/Renderer/Renderer.h"

#include "nh-core/System/Memory.h"
#include "nh-core/System/Thread.h"
#include "nh-core/System/System.h"
#include "nh-core/Config/Config.h"

#include "nh-encoding/Encodings/UTF32.h"
#include "nh-encoding/Encodings/UTF8.h"

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>

// FUNCTIONS =========================================================================================== 

bool tk_core_handleBottombarMove(
    tk_core_Session *Session_p, int cCol, int cRow)
{
    int prev = Session_p->hover3;
    
    if (cRow == -1) {
        Session_p->hover3 = cCol;
        if (Session_p->Config.Sidebar.on) {Session_p->hover3 += 2;}
    } else {
        Session_p->hover3 = -1;
    }

    return prev != Session_p->hover3;
}

void tk_core_handleBottombarHit( 
    tk_core_Session *Session_p, nh_api_MouseEvent Event, int cCol) 
{
//    if (Event.trigger == NH_API_TRIGGER_PRESS && cCol < 45) {
//        tk_core_Session *Session_p = nh_core_getWorkloadArg();
//        if (cCol < Session_p->BottomBar.count + 2) {
//            tk_core_MacroWindow *Window_p = tk_core_insertAndFocusWindow(Session_p, cCol / 5);
//            Window_p->refreshGrid2 = true; 
//            Window_p->refreshBottomBar = true; 
//            Window_p->Tile_p->refresh = true; 
//        }
//    } 
//
//    if (Session_p->Config.Sidebar.on) {cCol += 2;}
//
//    tk_core_View *View_p = Session_p->Views.pp[0];
//    tk_core_Row *Row_p = &(View_p->Grid1_p[View_p->rows]);
//
//    if (Row_p->Glyphs_p[cCol].codepoint == 0x2261) {
//        nh_core_overwriteGlobalConfigSettingInt(Session_p->namespace_p, "tk-core.sidebar", !Session_p->Config.Sidebar.on);
//    } 
//    if (cCol < View_p->cols && Row_p->Glyphs_p[cCol+1].codepoint == 0x2261) {
//        nh_core_overwriteGlobalConfigSettingInt(Session_p->namespace_p, "tk-core.sidebar", !Session_p->Config.Sidebar.on);
//    }
//    if (cCol > 0 && Row_p->Glyphs_p[cCol-1].codepoint == 0x2261) {
//        nh_core_overwriteGlobalConfigSettingInt(Session_p->namespace_p, "tk-core.sidebar", !Session_p->Config.Sidebar.on);
//    }
} 

TK_API_RESULT tk_core_drawBottombar(
    tk_core_Session *Session_p)
{
    tk_core_View *View_p = Session_p->Views.pp[0];
    tk_core_Row *Row_p = &(View_p->Grid1_p[View_p->rows+1]);
    tk_core_Glyph *Glyphs_p = Row_p->Glyphs_p;
    int cols = View_p->cols;
//    if (Session_p->Config.BottomBar.on == false) {return TK_API_SUCCESS;}

    // Set titlebar color.
    for (int i = 0; i < cols ; ++i) {
        memset(&Row_p->Glyphs_p[i], 0, sizeof(tk_core_Glyph));
        Row_p->Glyphs_p[i].mark |= TK_CORE_MARK_ACCENT;
        Row_p->Glyphs_p[i].mark |= TK_CORE_MARK_ACCENT_BACKGROUND_2;
        Row_p->Glyphs_p[i].mark |= TK_CORE_MARK_FOOTER;
        Row_p->Glyphs_p[i].Background.custom = true;

        if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.highContrast) {
            Row_p->Glyphs_p[i].Attributes.reverse = true;
        }

        Row_p->update_p[i] = true;
    }


    TK_CORE_MARK_E mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_FOOTER;
    TK_CORE_MARK_E background = Session_p->hover3 >= cols - 3 && Session_p->hover3 <= cols - 1 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    if (background & TK_CORE_MARK_ACCENT_BACKGROUND) {
        // for high-contrast
        Glyphs_p[cols - 1].Attributes.reverse = false;
        Glyphs_p[cols - 2].Attributes.reverse = false;
        Glyphs_p[cols - 3].Attributes.reverse = false;
    }

    Row_p->Glyphs_p[cols - 1].mark = mark | background;
    Row_p->update_p[cols - 1] = true;
    Row_p->Glyphs_p[cols - 1].Background.custom = true;

    Row_p->Glyphs_p[cols - 2].mark = mark | background;
    Row_p->Glyphs_p[cols - 2].Attributes.bold = true;
    Row_p->Glyphs_p[cols - 2].codepoint = 'S';
    Row_p->update_p[cols - 2] = true;
    Row_p->Glyphs_p[cols - 2].Background.custom = true;

    Row_p->Glyphs_p[cols - 3].mark = mark | background;
    Row_p->update_p[cols - 3] = true;
    Row_p->Glyphs_p[cols - 3].Background.custom = true;

    background = Session_p->hover3 >= cols - 6 && Session_p->hover3 <= cols - 4 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    if (background & TK_CORE_MARK_ACCENT_BACKGROUND) {
        // for high-contrast
        Glyphs_p[cols - 4].Attributes.reverse = false;
        Glyphs_p[cols - 5].Attributes.reverse = false;
        Glyphs_p[cols - 6].Attributes.reverse = false;
    }

    Row_p->Glyphs_p[cols - 4].mark = mark | background;
    Row_p->update_p[cols - 4] = true;
    Row_p->Glyphs_p[cols - 4].Background.custom = true;

    Row_p->Glyphs_p[cols - 5].codepoint = 'I';
    Row_p->Glyphs_p[cols - 5].mark = mark | background;

    Row_p->Glyphs_p[cols - 6].mark = mark | background;
    Row_p->update_p[cols - 6] = true;
    Row_p->Glyphs_p[cols - 6].Background.custom = true;

    background = Session_p->hover3 >= cols - 9 && Session_p->hover3 <= cols - 7 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    if (background == TK_CORE_MARK_ACCENT_BACKGROUND) {
        // for high-contrast
        Glyphs_p[cols - 7].Attributes.reverse = false;
        Glyphs_p[cols - 8].Attributes.reverse = false;
        Glyphs_p[cols - 9].Attributes.reverse = false;
    }

    Row_p->Glyphs_p[cols - 7].mark = mark | background;
    Row_p->update_p[cols - 7] = true;
    Row_p->Glyphs_p[cols - 7].Background.custom = true;

    Row_p->Glyphs_p[cols - 8].mark = mark | background;
    Row_p->Glyphs_p[cols - 8].Attributes.bold = true;
    Row_p->Glyphs_p[cols - 8].codepoint = 'C';
    Row_p->update_p[cols - 8] = true;
    Row_p->Glyphs_p[cols - 8].Background.custom = true;

    Row_p->Glyphs_p[cols - 9].mark = mark | background;
    Row_p->update_p[cols - 9] = true;
    Row_p->Glyphs_p[cols - 9].Background.custom = true;

    background = (Session_p->hover3 >= cols - 12 && Session_p->hover3 <= cols - 10) || Session_p->Config.Sidebar.on ? TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    if (background & TK_CORE_MARK_ACCENT_BACKGROUND) {
        // for high-contrast
        Glyphs_p[cols - 10].Attributes.reverse = false;
        Glyphs_p[cols - 11].Attributes.reverse = false;
        Glyphs_p[cols - 12].Attributes.reverse = false;
    }

    Row_p->Glyphs_p[cols - 10].mark = mark | background;
    Row_p->update_p[cols - 10] = true;
    Row_p->Glyphs_p[cols - 10].Background.custom = true;

    Row_p->Glyphs_p[cols - 11].mark = mark | background;
    Row_p->Glyphs_p[cols - 11].Attributes.bold = true;
    Row_p->Glyphs_p[cols - 11].codepoint = 'B';
    Row_p->update_p[cols - 11] = true;
    Row_p->Glyphs_p[cols - 11].Background.custom = true;

    Row_p->Glyphs_p[cols - 12].mark = mark | background;
    Row_p->update_p[cols - 12] = true;
    Row_p->Glyphs_p[cols - 12].Background.custom = true;

    int offset = Session_p->Config.Sidebar.on ? 2 : 0;

    for (int i = 0; i < Session_p->Windows.size; ++i) 
    {
        tk_core_MacroWindow *Window_p = Session_p->Windows.pp[i];
        int label = strlen(Window_p->type_p) + 4;

        for (int j = label; j < label; ++j) {
            Row_p->update_p[j+offset] = true;
        }
 
        background = Session_p->hover3 >= offset - 2 && Session_p->hover3 <= offset + 2 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

        if (Window_p == Session_p->Window_p) {
            background = TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_HIGHLIGHT;
        }

//        if (background & TK_CORE_MARK_ACCENT_BACKGROUND) {
//            for (int j = 0; j < label; ++j) {
//                // for high-contrast
//                Glyphs_p[j+offset].Attributes.reverse = false;
//            }
//        }

        for (int j = 0; j < label; ++j) {
            Glyphs_p[j+offset].codepoint = 0;
            Glyphs_p[j+offset].mark |= mark | background;
            Glyphs_p[j+offset].Background.custom = true;
        }
 
        for (int j = 0; j < strlen(Window_p->type_p); ++j) {
            Glyphs_p[j+offset+2].Attributes.bold = true;
            Glyphs_p[j+offset+2].codepoint = Window_p->type_p[j];
        }

        offset += strlen(Window_p->type_p) + 4;
    }

    offset += 2;

    Row_p->update_p[offset - 2] = true;
    Row_p->update_p[offset - 1] = true;
    Row_p->update_p[offset] = true;
    Row_p->update_p[offset + 1] = true;
    Row_p->update_p[offset + 2] = true;

    background = Session_p->hover3 >= offset - 2 && Session_p->hover3 <= offset + 2 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    Glyphs_p[offset - 2].codepoint = 0;
    Glyphs_p[offset - 2].mark = mark | background;
    Glyphs_p[offset - 2].Background.custom = true;
 
    Glyphs_p[offset - 1].codepoint = 0;
    Glyphs_p[offset - 1].mark = mark | background;
    Glyphs_p[offset - 1].Background.custom = true;
 
    Glyphs_p[offset].codepoint = '+';
    Glyphs_p[offset].Attributes.bold = true;
    Glyphs_p[offset].Background.custom = true;
    Glyphs_p[offset].mark |= mark | background;
 
    Glyphs_p[offset + 1].Background.custom = true;
    Glyphs_p[offset + 1].mark = mark | background;
    Glyphs_p[offset + 1].codepoint = 0;
 
    Glyphs_p[offset + 2].Background.custom = true;
    Glyphs_p[offset + 2].mark = mark | background;
    Glyphs_p[offset + 2].codepoint = 0;
 
    if (background == TK_CORE_MARK_ACCENT_BACKGROUND) {
        // for high-contrast
        Glyphs_p[offset - 2].Attributes.reverse = false;
        Glyphs_p[offset - 1].Attributes.reverse = false;
        Glyphs_p[offset - 0].Attributes.reverse = false;
        Glyphs_p[offset + 1].Attributes.reverse = false;
        Glyphs_p[offset + 2].Attributes.reverse = false;
    }

    return TK_API_SUCCESS;
}
