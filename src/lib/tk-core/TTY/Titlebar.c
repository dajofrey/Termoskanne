// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Titlebar.h"
#include "Program.h"
#include "TTY.h"

#include "../Common/Macros.h"

#include "../../tk-terminal/Terminal/Terminal.h"

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

bool tk_core_handleTitlebarMove(
    tk_core_TTY *TTY_p, int cCol, int cRow)
{
    int prev = TTY_p->Titlebar.hover;
    
    if (cRow == -1) {
        TTY_p->Titlebar.hover = cCol;
        if (TTY_p->Config.Sidebar.on) {TTY_p->Titlebar.hover += 2;}
    } else {
        TTY_p->Titlebar.hover = -1;
    }

    return prev != TTY_p->Titlebar.hover;
}

void tk_core_handleTitlebarHit( 
    tk_core_TTY *TTY_p, nh_api_MouseEvent Event, int cCol) 
{
    if (Event.trigger == NH_API_TRIGGER_PRESS && cCol < 45) {
        tk_core_TTY *TTY_p = nh_core_getWorkloadArg();
        if (cCol < TTY_p->Titlebar.count + 2) {
            tk_core_MacroWindow *Window_p = tk_core_insertAndFocusWindow(TTY_p, cCol / 5);
            Window_p->refreshGrid2 = true; 
            Window_p->refreshTitlebar = true; 
            Window_p->Tile_p->refresh = true; 
        }
    } 

    if (TTY_p->Config.Sidebar.on) {cCol += 2;}

    tk_core_View *View_p = TTY_p->Views.pp[0];
    tk_core_Row *Row_p = &(View_p->Grid1_p[View_p->rows]);

    if (Row_p->Glyphs_p[cCol].codepoint == 0x2261) {
        nh_core_overwriteGlobalConfigSettingInt(TTY_p->namespace_p, "tk-core.sidebar", !TTY_p->Config.Sidebar.on);
    } 
    if (cCol < View_p->cols && Row_p->Glyphs_p[cCol+1].codepoint == 0x2261) {
        nh_core_overwriteGlobalConfigSettingInt(TTY_p->namespace_p, "tk-core.sidebar", !TTY_p->Config.Sidebar.on);
    }
    if (cCol > 0 && Row_p->Glyphs_p[cCol-1].codepoint == 0x2261) {
        nh_core_overwriteGlobalConfigSettingInt(TTY_p->namespace_p, "tk-core.sidebar", !TTY_p->Config.Sidebar.on);
    }
} 

void tk_core_checkTitlebar(
    tk_core_Config *Config_p, tk_core_Titlebar *Titlebar_p, bool *refresh_p)
{
    if (Config_p->Titlebar.on == false) {return;}

    if (NH_SYSTEM.LocalTime.days != Titlebar_p->Time.days || NH_SYSTEM.LocalTime.hours != Titlebar_p->Time.hours
    ||  NH_SYSTEM.LocalTime.minutes != Titlebar_p->Time.minutes || NH_SYSTEM.RAM.free != Titlebar_p->RAM.free) {
        *refresh_p = true;
    }
}

TK_CORE_RESULT tk_core_drawTitlebar(
    tk_core_TTY *TTY_p)
{
    tk_core_View *View_p = TTY_p->Views.pp[0];
    tk_core_Row *Row_p = &(View_p->Grid1_p[View_p->rows]);
    tk_core_Glyph *Glyphs_p = Row_p->Glyphs_p;
    int cols = View_p->cols;
    if (TTY_p->Config.Titlebar.on == false) {return TK_CORE_SUCCESS;}

    // Set titlebar color.
    for (int i = 0; i < cols ; ++i) {
        memset(&Row_p->Glyphs_p[i], 0, sizeof(tk_core_Glyph));
        Row_p->Glyphs_p[i].mark |= TK_CORE_MARK_ACCENT;
        Row_p->Glyphs_p[i].mark |= TK_CORE_MARK_ACCENT_BACKGROUND_2;
        Row_p->Glyphs_p[i].Background.custom = true;

        if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.highContrast) {
            Row_p->Glyphs_p[i].Attributes.reverse = true;
        }

        Row_p->update_p[i] = true;
    }

    TK_CORE_MARK_E background = TTY_p->Titlebar.hover >= cols - 3 && TTY_p->Titlebar.hover <= cols - 1 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    if (background & TK_CORE_MARK_ACCENT_BACKGROUND) {
        // for high-contrast
        Glyphs_p[cols - 1].Attributes.reverse = false;
        Glyphs_p[cols - 2].Attributes.reverse = false;
        Glyphs_p[cols - 3].Attributes.reverse = false;
    }

    Row_p->Glyphs_p[cols - 1].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->update_p[cols - 1] = true;
    Row_p->Glyphs_p[cols - 1].Background.custom = true;

    Row_p->Glyphs_p[cols - 2].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->Glyphs_p[cols - 2].Attributes.bold = true;
    Row_p->Glyphs_p[cols - 2].codepoint = 0x00d7;
    Row_p->update_p[cols - 2] = true;
    Row_p->Glyphs_p[cols - 2].Background.custom = true;

    Row_p->Glyphs_p[cols - 3].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->update_p[cols - 3] = true;
    Row_p->Glyphs_p[cols - 3].Background.custom = true;

    background = TTY_p->Titlebar.hover >= cols - 6 && TTY_p->Titlebar.hover <= cols - 4 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    if (background & TK_CORE_MARK_ACCENT_BACKGROUND) {
        // for high-contrast
        Glyphs_p[cols - 4].Attributes.reverse = false;
        Glyphs_p[cols - 5].Attributes.reverse = false;
        Glyphs_p[cols - 6].Attributes.reverse = false;
    }

    Row_p->Glyphs_p[cols - 4].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->update_p[cols - 4] = true;
    Row_p->Glyphs_p[cols - 4].Background.custom = true;

    Row_p->Glyphs_p[cols - 5].codepoint = 0x25bc;
    Row_p->Glyphs_p[cols - 5].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->update_p[cols - 5] = true;
    Row_p->Glyphs_p[cols - 5].Background.custom = true;

    Row_p->Glyphs_p[cols - 6].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->update_p[cols - 6] = true;
    Row_p->Glyphs_p[cols - 6].Background.custom = true;

    background = TTY_p->Titlebar.hover >= cols - 9 && TTY_p->Titlebar.hover <= cols - 7 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    if (background == TK_CORE_MARK_ACCENT_BACKGROUND) {
        // for high-contrast
        Glyphs_p[cols - 7].Attributes.reverse = false;
        Glyphs_p[cols - 8].Attributes.reverse = false;
        Glyphs_p[cols - 9].Attributes.reverse = false;
    }

    Row_p->Glyphs_p[cols - 7].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->update_p[cols - 7] = true;
    Row_p->Glyphs_p[cols - 7].Background.custom = true;

    Row_p->Glyphs_p[cols - 8].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->Glyphs_p[cols - 8].Attributes.bold = true;
    Row_p->Glyphs_p[cols - 8].codepoint = 0x25b2;
    Row_p->update_p[cols - 8] = true;
    Row_p->Glyphs_p[cols - 8].Background.custom = true;

    Row_p->Glyphs_p[cols - 9].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->update_p[cols - 9] = true;
    Row_p->Glyphs_p[cols - 9].Background.custom = true;

    background = (TTY_p->Titlebar.hover >= cols - 12 && TTY_p->Titlebar.hover <= cols - 10) || TTY_p->Config.Sidebar.on ? TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    if (background & TK_CORE_MARK_ACCENT_BACKGROUND) {
        // for high-contrast
        Glyphs_p[cols - 10].Attributes.reverse = false;
        Glyphs_p[cols - 11].Attributes.reverse = false;
        Glyphs_p[cols - 12].Attributes.reverse = false;
    }

    Row_p->Glyphs_p[cols - 10].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->update_p[cols - 10] = true;
    Row_p->Glyphs_p[cols - 10].Background.custom = true;

    Row_p->Glyphs_p[cols - 11].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->Glyphs_p[cols - 11].Attributes.bold = true;
    Row_p->Glyphs_p[cols - 11].codepoint = 0x2261;
    Row_p->update_p[cols - 11] = true;
    Row_p->Glyphs_p[cols - 11].Background.custom = true;

    Row_p->Glyphs_p[cols - 12].mark = TK_CORE_MARK_ACCENT | background;
    Row_p->update_p[cols - 12] = true;
    Row_p->Glyphs_p[cols - 12].Background.custom = true;

    int count = TTY_p->Config.Sidebar.on ? 4 : 2;
    for (int i = 0; i < TTY_p->Windows.size; ++i) {

        tk_core_MacroWindow *Window_p = TTY_p->Windows.pp[i];
        Row_p->Glyphs_p[count].codepoint = i + '1';

        Row_p->update_p[count - 2] = true;
        Row_p->update_p[count - 1] = true;
        Row_p->update_p[count] = true;
        Row_p->update_p[count + 1] = true;
        Row_p->update_p[count + 2] = true;
 
        background = TTY_p->Titlebar.hover >= count - 2 && TTY_p->Titlebar.hover <= count + 2 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

        if (Window_p == TTY_p->Window_p) {
            background = TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_HIGHLIGHT;
        }

        if (background & TK_CORE_MARK_ACCENT_BACKGROUND) {
            // for high-contrast
            Glyphs_p[count - 2].Attributes.reverse = false;
            Glyphs_p[count - 1].Attributes.reverse = false;
            Glyphs_p[count - 0].Attributes.reverse = false;
            Glyphs_p[count + 1].Attributes.reverse = false;
            Glyphs_p[count + 2].Attributes.reverse = false;
        }

        Glyphs_p[count - 2].codepoint = 0;
        Glyphs_p[count - 2].mark |= TK_CORE_MARK_ACCENT | background;
        Glyphs_p[count - 2].Background.custom = true;
 
        Glyphs_p[count - 1].codepoint = 0;
        Glyphs_p[count - 1].mark |= TK_CORE_MARK_ACCENT | background;
        Glyphs_p[count - 1].Background.custom = true;
 
        Glyphs_p[count].Attributes.bold = true;
        Glyphs_p[count].Background.custom = true;
        Glyphs_p[count].mark = TK_CORE_MARK_ACCENT | background;
 
        Glyphs_p[count + 1].Background.custom = true;
        Glyphs_p[count + 1].mark = TK_CORE_MARK_ACCENT | background;
        Glyphs_p[count + 1].codepoint = 0;
 
        Glyphs_p[count + 2].Background.custom = true;
        Glyphs_p[count + 2].mark = TK_CORE_MARK_ACCENT | background;
        Glyphs_p[count + 2].codepoint = 0;

        count += 5;
    }

    Row_p->update_p[count - 2] = true;
    Row_p->update_p[count - 1] = true;
    Row_p->update_p[count] = true;
    Row_p->update_p[count + 1] = true;
    Row_p->update_p[count + 2] = true;
 
    background = TTY_p->Titlebar.hover >= count - 2 && TTY_p->Titlebar.hover <= count + 2 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    Glyphs_p[count - 2].codepoint = 0;
    Glyphs_p[count - 2].mark = TK_CORE_MARK_ACCENT | background;
    Glyphs_p[count - 2].Background.custom = true;
 
    Glyphs_p[count - 1].codepoint = 0;
    Glyphs_p[count - 1].mark = TK_CORE_MARK_ACCENT | background;
    Glyphs_p[count - 1].Background.custom = true;
 
    Glyphs_p[count].codepoint = '+';
    Glyphs_p[count].Attributes.bold = true;
    Glyphs_p[count].Background.custom = true;
    Glyphs_p[count].mark |= TK_CORE_MARK_ACCENT | background;
 
    Glyphs_p[count + 1].Background.custom = true;
    Glyphs_p[count + 1].mark = TK_CORE_MARK_ACCENT | background;
    Glyphs_p[count + 1].codepoint = 0;
 
    Glyphs_p[count + 2].Background.custom = true;
    Glyphs_p[count + 2].mark = TK_CORE_MARK_ACCENT | background;
    Glyphs_p[count + 2].codepoint = 0;
 
    if (background == TK_CORE_MARK_ACCENT_BACKGROUND) {
        // for high-contrast
        Glyphs_p[count - 2].Attributes.reverse = false;
        Glyphs_p[count - 1].Attributes.reverse = false;
        Glyphs_p[count - 0].Attributes.reverse = false;
        Glyphs_p[count + 1].Attributes.reverse = false;
        Glyphs_p[count + 2].Attributes.reverse = false;
    }

    TTY_p->Titlebar.count = count;

    if (TTY_p->Config.Sidebar.on) {
        Row_p->update_p[0] = 1;
        Row_p->update_p[1] = 1;
        Row_p->Glyphs_p[0].codepoint = 0x263c;
        Row_p->Glyphs_p[0].Attributes.bold = true;
        if (TTY_p->Sidebar.hover == -1) {
            Row_p->Glyphs_p[0].mark |= TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_ACCENT | TK_CORE_MARK_SIDEBAR;
            Row_p->Glyphs_p[1].mark |= TK_CORE_MARK_HIGHLIGHT | TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_SIDEBAR;
            if (((tk_terminal_Terminal*)TTY_p->Terminal_p)->Config.highContrast) {
                Row_p->Glyphs_p[0].Attributes.reverse = false;
                Row_p->Glyphs_p[1].Attributes.reverse = false;
            }
        } else {
            Row_p->Glyphs_p[0].mark |= TK_CORE_MARK_ACCENT_BACKGROUND_2 | TK_CORE_MARK_ACCENT | TK_CORE_MARK_SIDEBAR;
            Row_p->Glyphs_p[1].mark |= TK_CORE_MARK_ACCENT_BACKGROUND_2 | TK_CORE_MARK_SIDEBAR;
        }
        Row_p->Glyphs_p[0].Background.custom = true;
        Row_p->Glyphs_p[1].Background.custom = true;
    }

    return TK_CORE_SUCCESS;
}
