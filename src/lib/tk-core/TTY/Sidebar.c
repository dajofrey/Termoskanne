// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Sidebar.h"
#include "Program.h"
#include "TTY.h"

#include "../Common/Macros.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

// FUNCTIONS =======================================================================================

bool tk_core_handleSidebarMove( 
    tk_core_TTY *TTY_p, int cCol, int cRow) 
{
    tk_core_View *View_p = TTY_p->Views.pp[0];

    int prev = TTY_p->Sidebar.hover;

    if ((cCol == 0 || cCol == 1) && cRow < 4) {
        TTY_p->Sidebar.hover = cRow;
    } else {
        TTY_p->Sidebar.hover = -999;
    }

    return prev != TTY_p->Sidebar.hover;
} 

//void tk_core_handleSidebarHit( 
//    nh_api_MouseEvent Event, int cCol) 
//{
//    if (Event.trigger == NH_API_TRIGGER_PRESS && cCol-1 < 9) { 
//        tk_core_MacroWindow *Window_p = tk_core_insertAndFocusWindow(nh_core_getWorkloadArg(), cCol-1);
//        Window_p->refreshGrid2 = true; 
//        Window_p->refreshTitlebar = true; 
//        Window_p->Tile_p->refresh = true; 
//    } 
//} 

TK_CORE_RESULT tk_core_drawSidebar(
    tk_core_TTY *TTY_p)
{
    tk_core_View *View_p = TTY_p->Views.pp[0];

    for (int row = 0; row < View_p->rows; ++row) {
        memset(View_p->Grid1_p[row].Glyphs_p, 0, sizeof(tk_core_Glyph));
        memset(View_p->Grid1_p[row].Glyphs_p+1, 0, sizeof(tk_core_Glyph));

        if (TTY_p->Sidebar.hover == row) {
            View_p->Grid1_p[row].Glyphs_p[0].mark = TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_SIDEBAR;
            View_p->Grid1_p[row].Glyphs_p[1].mark = TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_SIDEBAR;
        } else {
            View_p->Grid1_p[row].Glyphs_p[0].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2 | TK_CORE_MARK_SIDEBAR;
            View_p->Grid1_p[row].Glyphs_p[1].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2 | TK_CORE_MARK_SIDEBAR;
        } 

        View_p->Grid1_p[row].Glyphs_p[0].Background.custom = 1;
        View_p->Grid1_p[row].Glyphs_p[1].Background.custom = 1;
 
        View_p->Grid1_p[row].update_p[0] = true;
        View_p->Grid1_p[row].update_p[1] = true;
    }

    View_p->Grid1_p[0].Glyphs_p[0].codepoint = '-';
    View_p->Grid1_p[0].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
    View_p->Grid1_p[0].Glyphs_p[0].Attributes.bold = true;

    View_p->Grid1_p[1].Glyphs_p[0].codepoint = '+';
    View_p->Grid1_p[1].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
    View_p->Grid1_p[1].Glyphs_p[0].Attributes.bold = true;

    View_p->Grid1_p[2].Glyphs_p[0].codepoint = 0x25b6;
    View_p->Grid1_p[2].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
    View_p->Grid1_p[2].Glyphs_p[0].Attributes.bold = true;

    View_p->Grid1_p[3].Glyphs_p[0].codepoint = 0x03d9;
    View_p->Grid1_p[3].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
    View_p->Grid1_p[3].Glyphs_p[0].Attributes.bold = true;

//    View_p->Grid1_p[3].Glyphs_p[0].codepoint = 0x2605;
//    View_p->Grid1_p[3].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
//    View_p->Grid1_p[3].Glyphs_p[0].Attributes.bold = true;

    return TK_CORE_SUCCESS;
}
