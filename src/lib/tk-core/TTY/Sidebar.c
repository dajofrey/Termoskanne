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

    View_p->Grid1_p[0].Glyphs_p[0].Attributes.reverse = false;
    View_p->Grid1_p[0].Glyphs_p[0].Attributes.bold = true;
    View_p->Grid1_p[0].Glyphs_p[0].codepoint = 0x2261;
    View_p->Grid1_p[0].Glyphs_p[1].mark |= TK_CORE_MARK_LINE_VERTICAL | TK_CORE_MARK_LINE_GRAPHICS | TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND;
    View_p->Grid1_p[0].Glyphs_p[1].Background.custom = true;
    View_p->Grid1_p[0].Glyphs_p[1].Attributes.reverse = false;
    View_p->Grid1_p[0].Glyphs_p[1].codepoint = 'x';
    
    View_p->Grid1_p[View_p->rows/2-4].Glyphs_p[0].codepoint = 0x25cf;
    View_p->Grid1_p[View_p->rows/2-3].Glyphs_p[0].codepoint = 0x25cf;
    View_p->Grid1_p[View_p->rows/2-2].Glyphs_p[0].codepoint = 0x25cf;
    View_p->Grid1_p[View_p->rows/2-1].Glyphs_p[0].codepoint = 0x25cf;
    View_p->Grid1_p[View_p->rows/2  ].Glyphs_p[0].codepoint = 0x25cf;
    View_p->Grid1_p[View_p->rows/2+1].Glyphs_p[0].codepoint = 0x25cf;
    View_p->Grid1_p[View_p->rows/2+2].Glyphs_p[0].codepoint = 0x25cf;
    View_p->Grid1_p[View_p->rows/2+3].Glyphs_p[0].codepoint = 0x25cf;
    View_p->Grid1_p[View_p->rows/2+4].Glyphs_p[0].codepoint = 0x25cf;

    unsigned int focus = View_p->rows/2-4 + tk_core_getCurrentWindowIndex(TTY_p);
    View_p->Grid1_p[focus].Glyphs_p[0].codepoint = 0x25cb;

    for (int i = 0; i < View_p->rows; ++i) {
        if (View_p->Grid1_p[i].Glyphs_p[2].Attributes.reverse == true) {
            View_p->Grid1_p[i].Glyphs_p[1].mark |= TK_CORE_MARK_LINE_GRAPHICS | TK_CORE_MARK_ACCENT;
            View_p->Grid1_p[i].Glyphs_p[1].Attributes.reverse = false;
            View_p->Grid1_p[i].Glyphs_p[1].codepoint = 'd';
        } 
        if (TTY_p->Config.Sidebar.solid) {
            View_p->Grid1_p[i].Glyphs_p[0].Attributes.reverse = true;
            View_p->Grid1_p[i].Glyphs_p[1].codepoint = 'e';
            View_p->Grid1_p[i].Glyphs_p[0].Attributes.bold = true;
            if (View_p->Grid1_p[i].Glyphs_p[2].Background.custom = true && View_p->Grid1_p[i].Glyphs_p[2].mark & TK_CORE_MARK_ACCENT_BACKGROUND) {
                View_p->Grid1_p[i].Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT_BACKGROUND;
                View_p->Grid1_p[i].Glyphs_p[1].Background.custom = true;
            }
        }
        View_p->Grid1_p[i].update_p[1] = true;
        View_p->Grid1_p[i].update_p[0] = true;
    }

    return TK_CORE_SUCCESS;
}
