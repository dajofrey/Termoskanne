// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Sidebar.h"
#include "Program.h"
#include "Session.h"

#include "../Common/Macros.h"

#include "../../tk-gfx/Renderer/Renderer.h"
#include "../../tk-gfx/Common/Config.h"

#include "nh-core/Config/Config.h"

#include <unistd.h>
#include <stdio.h>
#include <string.h>

// FUNCTIONS =======================================================================================

bool tk_core_handleSidebarMove( 
    tk_core_Session *Session_p, int cCol, int cRow) 
{
    tk_core_View *View_p = Session_p->Views.pp[0];

    int prev = Session_p->Sidebar.hover;

    if ((cCol == -2 || cCol == -1)) {
        Session_p->Sidebar.hover = cRow;
    } else {
        Session_p->Sidebar.hover = -999;
    }

    return prev != Session_p->Sidebar.hover;
} 

void tk_core_handleSidebarHit( 
    tk_core_Session *Session_p, nh_api_MouseEvent Event, int cRow) 
{
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 1) {
        nh_core_overwriteGlobalConfigSettingInt(((tk_gfx_Renderer*)Session_p->Renderer_p)->namespace_p, "tk-gfx.font.size", ((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.fontSize+1);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 2) {
        nh_core_overwriteGlobalConfigSettingInt(((tk_gfx_Renderer*)Session_p->Renderer_p)->namespace_p, "tk-gfx.font.size", ((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.fontSize-1);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 3) {
        float animate = 0.0f;
        if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.animationFreq == 0) {
            animate = 1.0f;
        }
        if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.animationFreq == 1.0f) {
            animate = 2.0f;
        }
        if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.animationFreq == 2.0f) {
            animate = 0;
        }
        nh_core_overwriteGlobalConfigSettingInt(((tk_gfx_Renderer*)Session_p->Renderer_p)->namespace_p, "tk-gfx.animate", animate);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == ((tk_core_View*)Session_p->Views.pp[0])->rows-4) {
        int style = ((((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.style)+1) % 7;
        nh_core_overwriteGlobalConfigSettingInt(((tk_gfx_Renderer*)Session_p->Renderer_p)->namespace_p, "tk-gfx.style", style);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == ((tk_core_View*)Session_p->Views.pp[0])->rows-3) {

        int r = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Backgrounds_p[0].r*255.0f);
        int g = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Backgrounds_p[0].g*255.0f);
        int b = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Backgrounds_p[0].b*255.0f);
        int a = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Backgrounds_p[0].a*255.0f);
        char background_p[255];
        sprintf(background_p, "%d,%d,%d,%d", r, g, b, a);

        nh_core_overwriteGlobalConfigSetting(((tk_gfx_Renderer*)Session_p->Renderer_p)->namespace_p, "tk-gfx.color.foreground", background_p);

        r = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Accents_p[0].r*255.0f);
        g = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Accents_p[0].g*255.0f);
        b = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Accents_p[0].b*255.0f);
        a = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Accents_p[0].a*255.0f);
        char accent_p[255];
        sprintf(accent_p, "%d,%d,%d,%d", r, g, b, a);
 
        r = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Foreground.r*255.0f);
        g = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Foreground.g*255.0f);
        b = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Foreground.b*255.0f);
        a = (int)(((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.Foreground.a*255.0f);
        char foreground_p[255];
        sprintf(foreground_p, "%d,%d,%d,%d", r, g, b, a);

        if (!strcmp(accent_p, foreground_p)) {
            nh_core_overwriteGlobalConfigSetting(((tk_gfx_Renderer*)Session_p->Renderer_p)->namespace_p, "tk-gfx.color.accent", background_p);
        }

        nh_core_overwriteGlobalConfigSetting(((tk_gfx_Renderer*)Session_p->Renderer_p)->namespace_p, "tk-gfx.color.background", foreground_p);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == ((tk_core_View*)Session_p->Views.pp[0])->rows-2) {
        nh_core_overwriteGlobalConfigSettingInt(((tk_gfx_Renderer*)Session_p->Renderer_p)->namespace_p, "tk-gfx.high_contrast", !((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.highContrast);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == ((tk_core_View*)Session_p->Views.pp[0])->rows-1) {
        int border = ((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.border;
        
        if (border < 2) {
            border++;
        } else {
            border = 0;
        }

        nh_core_overwriteGlobalConfigSettingInt(((tk_gfx_Renderer*)Session_p->Renderer_p)->namespace_p, "tk-gfx.border", border);
    }
    if (Event.trigger == NH_API_TRIGGER_PRESS && cRow == 9) {
        nh_core_overwriteGlobalConfigSettingInt(Session_p->namespace_p, "tk-core.titlebar.on", !Session_p->Config.Titlebar.on);
    }
} 

TK_API_RESULT tk_core_drawSidebar(
    tk_core_Session *Session_p)
{
    tk_core_View *View_p = Session_p->Views.pp[0];

    // prepare 

    for (int row = 0; row < View_p->rows; ++row) {
        memset(View_p->Grid1_p[row].Glyphs_p, 0, sizeof(tk_core_Glyph));
        memset(View_p->Grid1_p[row].Glyphs_p+1, 0, sizeof(tk_core_Glyph));

        if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.highContrast) {
            View_p->Grid1_p[row].Glyphs_p[0].Attributes.reverse = true;
            View_p->Grid1_p[row].Glyphs_p[1].Attributes.reverse = true;
        }

        if (Session_p->Sidebar.hover == row && View_p->Grid1_p[row].Glyphs_p[0].codepoint != 0 && View_p->Grid1_p[row].Glyphs_p[0].codepoint != ' ') {
            View_p->Grid1_p[row].Glyphs_p[0].mark = TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_SIDEBAR;
            View_p->Grid1_p[row].Glyphs_p[1].mark = TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_SIDEBAR;
            if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.highContrast) {
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

    // top
 
    TK_CORE_MARK_E background = Session_p->Sidebar.hover == 0 ? TK_CORE_MARK_HIGHLIGHT : 0;

    View_p->Grid1_p[0].Glyphs_p[0].codepoint = 0x0294;
    View_p->Grid1_p[0].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT | background;
    View_p->Grid1_p[0].Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT | background;
    View_p->Grid1_p[0].Glyphs_p[0].Attributes.bold = true;
    if (background == TK_CORE_MARK_HIGHLIGHT) {
        View_p->Grid1_p[0].Glyphs_p[0].Attributes.reverse = false;
        View_p->Grid1_p[0].Glyphs_p[1].Attributes.reverse = false;
    }
 
    background = Session_p->Sidebar.hover == 1 ? TK_CORE_MARK_HIGHLIGHT : 0;

    View_p->Grid1_p[1].Glyphs_p[0].codepoint = '+';
    View_p->Grid1_p[1].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT | background;
    View_p->Grid1_p[1].Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT | background;
    View_p->Grid1_p[1].Glyphs_p[0].Attributes.bold = true;
    if (background == TK_CORE_MARK_HIGHLIGHT) {
        View_p->Grid1_p[1].Glyphs_p[0].Attributes.reverse = false;
        View_p->Grid1_p[1].Glyphs_p[1].Attributes.reverse = false;
    }
 
    background = Session_p->Sidebar.hover == 2 ? TK_CORE_MARK_HIGHLIGHT : 0;

    View_p->Grid1_p[2].Glyphs_p[0].codepoint = '-';
    View_p->Grid1_p[2].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT | background;
    View_p->Grid1_p[2].Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT | background;
    View_p->Grid1_p[2].Glyphs_p[0].Attributes.bold = true;
    if (background == TK_CORE_MARK_HIGHLIGHT) {
        View_p->Grid1_p[2].Glyphs_p[0].Attributes.reverse = false;
        View_p->Grid1_p[2].Glyphs_p[1].Attributes.reverse = false;
    }
 
    if (Session_p->Config.Titlebar.on == true) {
        TK_CORE_MARK_E background = TK_CORE_MARK_ACCENT_BACKGROUND;
        View_p->Grid1_p[3].Glyphs_p[0].codepoint = '=';
        View_p->Grid1_p[3].Glyphs_p[0].mark = TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background | TK_CORE_MARK_HIGHLIGHT;
        View_p->Grid1_p[3].Glyphs_p[1].mark = TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background | TK_CORE_MARK_HIGHLIGHT;
        View_p->Grid1_p[3].Glyphs_p[0].Attributes.bold = true;
        if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.highContrast) {
            View_p->Grid1_p[3].Glyphs_p[0].Attributes.reverse = false;
            View_p->Grid1_p[3].Glyphs_p[1].Attributes.reverse = false;
        }
    } else {
        background = Session_p->Sidebar.hover == 3 ? TK_CORE_MARK_HIGHLIGHT : 0;
        View_p->Grid1_p[3].Glyphs_p[0].codepoint = '=';
        View_p->Grid1_p[3].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[3].Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[3].Glyphs_p[0].Attributes.bold = true;
        if (background == TK_CORE_MARK_HIGHLIGHT) {
            View_p->Grid1_p[3].Glyphs_p[0].Attributes.reverse = false;
            View_p->Grid1_p[3].Glyphs_p[1].Attributes.reverse = false;
        }
    }

    // bottom

    background = Session_p->Sidebar.hover == View_p->rows-5 ? TK_CORE_MARK_HIGHLIGHT : 0;

    if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.animationFreq != 0) {
        TK_CORE_MARK_E background = TK_CORE_MARK_ACCENT_BACKGROUND;
        View_p->Grid1_p[View_p->rows-5].Glyphs_p[0].codepoint = 0x25b6;
        View_p->Grid1_p[View_p->rows-5].Glyphs_p[0].mark = TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-5].Glyphs_p[1].mark = TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-5].Glyphs_p[0].Attributes.bold = true;
    } else {
        View_p->Grid1_p[View_p->rows-5].Glyphs_p[0].codepoint = 0x25b6;
        View_p->Grid1_p[View_p->rows-5].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-5].Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-5].Glyphs_p[0].Attributes.bold = true;
        if (background == TK_CORE_MARK_HIGHLIGHT) {
            View_p->Grid1_p[View_p->rows-5].Glyphs_p[0].Attributes.reverse = false;
            View_p->Grid1_p[View_p->rows-5].Glyphs_p[1].Attributes.reverse = false;
        }
    }

    background = Session_p->Sidebar.hover == View_p->rows-4 ? TK_CORE_MARK_HIGHLIGHT : 0;

    View_p->Grid1_p[View_p->rows-4].Glyphs_p[0].codepoint = 0x0405;
    View_p->Grid1_p[View_p->rows-4].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT | background;
    View_p->Grid1_p[View_p->rows-4].Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT | background;
    View_p->Grid1_p[View_p->rows-4].Glyphs_p[0].Attributes.bold = true;
    if (background == TK_CORE_MARK_HIGHLIGHT) {
        View_p->Grid1_p[View_p->rows-4].Glyphs_p[0].Attributes.reverse = false;
        View_p->Grid1_p[View_p->rows-4].Glyphs_p[1].Attributes.reverse = false;
    }
 
    background = Session_p->Sidebar.hover == View_p->rows-3 ? TK_CORE_MARK_HIGHLIGHT : 0;

    View_p->Grid1_p[View_p->rows-3].Glyphs_p[0].codepoint = 0x0399;
    View_p->Grid1_p[View_p->rows-3].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT | background;
    View_p->Grid1_p[View_p->rows-3].Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT | background;
    View_p->Grid1_p[View_p->rows-3].Glyphs_p[0].Attributes.bold = true;
    if (background == TK_CORE_MARK_HIGHLIGHT) {
        View_p->Grid1_p[View_p->rows-3].Glyphs_p[0].Attributes.reverse = false;
        View_p->Grid1_p[View_p->rows-3].Glyphs_p[1].Attributes.reverse = false;
    }
 
    if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.highContrast == true) {
        background = TK_CORE_MARK_ACCENT_BACKGROUND;
        View_p->Grid1_p[View_p->rows-2].Glyphs_p[0].codepoint = 'C';
        View_p->Grid1_p[View_p->rows-2].Glyphs_p[0].mark = TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-2].Glyphs_p[1].mark = TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-2].Glyphs_p[0].Attributes.bold = true;
        View_p->Grid1_p[View_p->rows-2].Glyphs_p[0].Attributes.reverse = false;
        View_p->Grid1_p[View_p->rows-2].Glyphs_p[1].Attributes.reverse = false;
    } else {
        background = Session_p->Sidebar.hover == View_p->rows-2 ? TK_CORE_MARK_HIGHLIGHT : 0;
        View_p->Grid1_p[View_p->rows-2].Glyphs_p[0].codepoint = 'C';
        View_p->Grid1_p[View_p->rows-2].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-2].Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-2].Glyphs_p[0].Attributes.bold = true;
        if (background == TK_CORE_MARK_HIGHLIGHT) {
            View_p->Grid1_p[View_p->rows-2].Glyphs_p[0].Attributes.reverse = false;
            View_p->Grid1_p[View_p->rows-2].Glyphs_p[1].Attributes.reverse = false;
        }
    }

    if (((tk_gfx_Renderer*)Session_p->Renderer_p)->Config.border > 0) {
        background = TK_CORE_MARK_ACCENT_BACKGROUND;
        View_p->Grid1_p[View_p->rows-1].Glyphs_p[0].codepoint = 'B';
        View_p->Grid1_p[View_p->rows-1].Glyphs_p[0].mark = TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-1].Glyphs_p[1].mark = TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_SIDEBAR | TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-1].Glyphs_p[0].Attributes.bold = true;
        View_p->Grid1_p[View_p->rows-1].Glyphs_p[0].Attributes.reverse = false;
        View_p->Grid1_p[View_p->rows-1].Glyphs_p[1].Attributes.reverse = false;
    } else {
        background = Session_p->Sidebar.hover == View_p->rows-1 ? TK_CORE_MARK_HIGHLIGHT : 0;
        View_p->Grid1_p[View_p->rows-1].Glyphs_p[0].codepoint = 'B';
        View_p->Grid1_p[View_p->rows-1].Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-1].Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT | background;
        View_p->Grid1_p[View_p->rows-1].Glyphs_p[0].Attributes.bold = true;
        if (background == TK_CORE_MARK_HIGHLIGHT) {
            View_p->Grid1_p[View_p->rows-1].Glyphs_p[0].Attributes.reverse = false;
            View_p->Grid1_p[View_p->rows-1].Glyphs_p[1].Attributes.reverse = false;
        }
    }

    return TK_API_SUCCESS;
}
