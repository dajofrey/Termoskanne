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

#include "../../tk-terminal/Terminal/Terminal.h"
#include "../../tk-terminal/Common/Config.h"

#include "nh-core/Config/Config.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

// FUNCTIONS =======================================================================================

bool tk_core_handleSidebarMove( 
    tk_core_TTY *TTY_p, int cCol, int cRow) 
{
    tk_core_View *View_p = TTY_p->Views.pp[0];

    int prev = TTY_p->Sidebar.hover;

    if ((cCol == -2 || cCol == -1) && cRow < 10) {
        TTY_p->Sidebar.hover = cRow;
    } else {
        TTY_p->Sidebar.hover = -999;
    }

    return prev != TTY_p->Sidebar.hover;
} 

void tk_core_handleSidebarHit( 
    tk_core_TTY *TTY_p, nh_api_MouseEvent Event, int cRow) 
{
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 1) {
        nh_core_overwriteGlobalConfigSettingInt(((tk_terminal_Terminal*)TTY_p->Terminal_p)->namespace_p, "tk-terminal.font.size", ((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.fontSize+1);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 2) {
        nh_core_overwriteGlobalConfigSettingInt(((tk_terminal_Terminal*)TTY_p->Terminal_p)->namespace_p, "tk-terminal.font.size", ((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.fontSize-1);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 3) {
        float animate = 0.0f;
        if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.animationFreq == 0) {
            animate = 1.0f;
        }
        if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.animationFreq == 1.0f) {
            animate = 2.0f;
        }
        if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.animationFreq == 2.0f) {
            animate = 0;
        }
        nh_core_overwriteGlobalConfigSettingInt(((tk_terminal_Terminal*)TTY_p->Terminal_p)->namespace_p, "tk-terminal.animate", animate);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 5) {
        int style = ((((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.style)+1) % 7;
        nh_core_overwriteGlobalConfigSettingInt(((tk_terminal_Terminal*)TTY_p->Terminal_p)->namespace_p, "tk-terminal.style", style);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 6) {

        int r = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Backgrounds_p[0].r*255.0f);
        int g = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Backgrounds_p[0].g*255.0f);
        int b = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Backgrounds_p[0].b*255.0f);
        int a = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Backgrounds_p[0].a*255.0f);
        char background_p[255];
        sprintf(background_p, "%d,%d,%d,%d", r, g, b, a);

        nh_core_overwriteGlobalConfigSetting(((tk_terminal_Terminal*)TTY_p->Terminal_p)->namespace_p, "tk-terminal.color.foreground", background_p);

        r = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Accents_p[0].r*255.0f);
        g = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Accents_p[0].g*255.0f);
        b = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Accents_p[0].b*255.0f);
        a = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Accents_p[0].a*255.0f);
        char accent_p[255];
        sprintf(accent_p, "%d,%d,%d,%d", r, g, b, a);
 
        r = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Foreground.r*255.0f);
        g = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Foreground.g*255.0f);
        b = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Foreground.b*255.0f);
        a = (int)(((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.Foreground.a*255.0f);
        char foreground_p[255];
        sprintf(foreground_p, "%d,%d,%d,%d", r, g, b, a);

        if (!strcmp(accent_p, foreground_p)) {
            nh_core_overwriteGlobalConfigSetting(((tk_terminal_Terminal*)TTY_p->Terminal_p)->namespace_p, "tk-terminal.color.accent", background_p);
        }

        nh_core_overwriteGlobalConfigSetting(((tk_terminal_Terminal*)TTY_p->Terminal_p)->namespace_p, "tk-terminal.color.background", foreground_p);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 7) {
        nh_core_overwriteGlobalConfigSettingInt(((tk_terminal_Terminal*)TTY_p->Terminal_p)->namespace_p, "tk-terminal.high_contrast", !((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.highContrast);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 8) {
        int border = ((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.border;
        
        if (border < 2) {
            border++;
        } else {
            border = 0;
        }

        nh_core_overwriteGlobalConfigSettingInt(((tk_terminal_Terminal*)TTY_p->Terminal_p)->namespace_p, "tk-terminal.border", border);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 9) {
        nh_core_overwriteGlobalConfigSettingInt(TTY_p->namespace_p, "tk-core.titlebar.on", !TTY_p->Config.Titlebar.on);
    }
} 

TK_CORE_RESULT tk_core_drawSidebar(
    tk_core_TTY *TTY_p)
{
    tk_core_View *View_p = TTY_p->Views.pp[0];

    for (int row = 0; row < View_p->rows; ++row) {
        memset(View_p->Grid1_p[row].Glyphs_p, 0, sizeof(tk_core_Glyph));
        memset(View_p->Grid1_p[row].Glyphs_p+1, 0, sizeof(tk_core_Glyph));

        if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.highContrast) {
            View_p->Grid1_p[row].Glyphs_p[0].Attributes.reverse = true;
            View_p->Grid1_p[row].Glyphs_p[1].Attributes.reverse = true;
        }

        if (TTY_p->Sidebar.hover == row) {
            View_p->Grid1_p[row].Glyphs_p[0].mark = TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_SIDEBAR;
            View_p->Grid1_p[row].Glyphs_p[1].mark = TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_SIDEBAR;
            if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.highContrast) {
                View_p->Grid1_p[row].Glyphs_p[0].Attributes.reverse = false;
                View_p->Grid1_p[row].Glyphs_p[1].Attributes.reverse = false;
            }
        } else {
            View_p->Grid1_p[row].Glyphs_p[0].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2 | TK_CORE_MARK_SIDEBAR;
            View_p->Grid1_p[row].Glyphs_p[1].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2 | TK_CORE_MARK_SIDEBAR;
        } 

        View_p->Grid1_p[row].Glyphs_p[0].Background.custom = 1;
        View_p->Grid1_p[row].Glyphs_p[1].Background.custom = 1;
 
        View_p->Grid1_p[row].update_p[0] = true;
        View_p->Grid1_p[row].update_p[1] = true;
    }

    if (TTY_p->Config.clean) {return TK_CORE_SUCCESS;}

    View_p->Grid1_p[0].Glyphs_p[0].codepoint = 0x0294;
    View_p->Grid1_p[0].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
    View_p->Grid1_p[0].Glyphs_p[0].Attributes.bold = true;

    View_p->Grid1_p[1].Glyphs_p[0].codepoint = '+';
    View_p->Grid1_p[1].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
    View_p->Grid1_p[1].Glyphs_p[0].Attributes.bold = true;

    View_p->Grid1_p[2].Glyphs_p[0].codepoint = '-';
    View_p->Grid1_p[2].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
    View_p->Grid1_p[2].Glyphs_p[0].Attributes.bold = true;

    if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.animationFreq != 0) {
        TK_CORE_MARK_E background = TK_CORE_MARK_ACCENT_BACKGROUND;
        View_p->Grid1_p[3].Glyphs_p[0].codepoint = 0x25b6;
        View_p->Grid1_p[3].Glyphs_p[0].mark = TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[3].Glyphs_p[1].mark = TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[3].Glyphs_p[0].Attributes.bold = true;
    } else {
        View_p->Grid1_p[3].Glyphs_p[0].codepoint = 0x25b6;
        View_p->Grid1_p[3].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
        View_p->Grid1_p[3].Glyphs_p[0].Attributes.bold = true;
    }

    View_p->Grid1_p[4].Glyphs_p[0].codepoint = 0x00a6;
    View_p->Grid1_p[4].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
    View_p->Grid1_p[4].Glyphs_p[0].Attributes.bold = true;

    View_p->Grid1_p[5].Glyphs_p[0].codepoint = 0x0405;
    View_p->Grid1_p[5].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
    View_p->Grid1_p[5].Glyphs_p[0].Attributes.bold = true;

    View_p->Grid1_p[6].Glyphs_p[0].codepoint = 0x0399;
    View_p->Grid1_p[6].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
    View_p->Grid1_p[6].Glyphs_p[0].Attributes.bold = true;

    if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.highContrast == true) {
        TK_CORE_MARK_E background = TK_CORE_MARK_ACCENT_BACKGROUND;
        View_p->Grid1_p[7].Glyphs_p[0].codepoint = 'C';
        View_p->Grid1_p[7].Glyphs_p[0].mark = TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[7].Glyphs_p[1].mark = TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[7].Glyphs_p[0].Attributes.bold = true;
        View_p->Grid1_p[7].Glyphs_p[0].Attributes.reverse = false;
        View_p->Grid1_p[7].Glyphs_p[1].Attributes.reverse = false;
    } else {
        View_p->Grid1_p[7].Glyphs_p[0].codepoint = 'C';
        View_p->Grid1_p[7].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
        View_p->Grid1_p[7].Glyphs_p[0].Attributes.bold = true;
    }

    if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.border > 0) {
        TK_CORE_MARK_E background = TK_CORE_MARK_ACCENT_BACKGROUND;
        View_p->Grid1_p[8].Glyphs_p[0].codepoint = 'B';
        View_p->Grid1_p[8].Glyphs_p[0].mark = TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[8].Glyphs_p[1].mark = TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[8].Glyphs_p[0].Attributes.bold = true;
        View_p->Grid1_p[8].Glyphs_p[0].Attributes.reverse = false;
        View_p->Grid1_p[8].Glyphs_p[1].Attributes.reverse = false;
    } else {
        View_p->Grid1_p[8].Glyphs_p[0].codepoint = 'B';
        View_p->Grid1_p[8].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
        View_p->Grid1_p[8].Glyphs_p[0].Attributes.bold = true;
    }


    if (TTY_p->Config.Titlebar.on == true) {
        TK_CORE_MARK_E background = TK_CORE_MARK_ACCENT_BACKGROUND;
        View_p->Grid1_p[9].Glyphs_p[0].codepoint = '=';
        View_p->Grid1_p[9].Glyphs_p[0].mark = TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[9].Glyphs_p[1].mark = TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[9].Glyphs_p[0].Attributes.bold = true;
        if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.highContrast) {
            View_p->Grid1_p[9].Glyphs_p[0].Attributes.reverse = false;
            View_p->Grid1_p[9].Glyphs_p[1].Attributes.reverse = false;
        }
    } else {
        View_p->Grid1_p[9].Glyphs_p[0].codepoint = '=';
        View_p->Grid1_p[9].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT;
        View_p->Grid1_p[9].Glyphs_p[0].Attributes.bold = true;
    }

    return TK_CORE_SUCCESS;
}
