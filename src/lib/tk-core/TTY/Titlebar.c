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

#include "nh-core/System/Memory.h"
#include "nh-core/System/Thread.h"
#include "nh-core/System/System.h"

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
    
    if (cRow == -1 && cCol < 45) {
        TTY_p->Titlebar.hover = cCol;
    } else {
        TTY_p->Titlebar.hover = -1;
    }

    return prev != TTY_p->Titlebar.hover;
}

void tk_core_handleTitlebarHit( 
    nh_api_MouseEvent Event, int cCol) 
{
    if (Event.trigger == NH_API_TRIGGER_PRESS && cCol < 45) {
        tk_core_TTY *TTY_p = nh_core_getWorkloadArg();
        if (cCol < 2 && TTY_p->Config.Sidebar.on) {return;}
        cCol = TTY_p->Config.Sidebar.on ? cCol - 2 : cCol;
        if (cCol < TTY_p->Titlebar.count + 2 && cCol > 2) {
            tk_core_MacroWindow *Window_p = tk_core_insertAndFocusWindow(TTY_p, cCol / 5);
            Window_p->refreshGrid2 = true; 
            Window_p->refreshTitlebar = true; 
            Window_p->Tile_p->refresh = true; 
        }
    } 
} 

// CHECK ===========================================================================================

void tk_core_checkTitlebar(
    tk_core_Config *Config_p, tk_core_Titlebar *Titlebar_p, bool *refresh_p)
{
    if (Config_p->Titlebar.on == false) {return;}

    if (NH_SYSTEM.LocalTime.days != Titlebar_p->Time.days || NH_SYSTEM.LocalTime.hours != Titlebar_p->Time.hours
    ||  NH_SYSTEM.LocalTime.minutes != Titlebar_p->Time.minutes || NH_SYSTEM.RAM.free != Titlebar_p->RAM.free) {
        *refresh_p = true;
    }
}

// DRAW ============================================================================================

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
        Row_p->Glyphs_p[i].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
        Row_p->update_p[i] = true;
        Row_p->Glyphs_p[i].Background.custom = true;
    }

    Row_p->Glyphs_p[cols - 1].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 1] = true;
    Row_p->Glyphs_p[cols - 1].Background.custom = true;

    Row_p->Glyphs_p[cols - 2].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->Glyphs_p[cols - 2].Attributes.bold = true;
    Row_p->Glyphs_p[cols - 2].codepoint = 0x00d7;
    Row_p->update_p[cols - 2] = true;
    Row_p->Glyphs_p[cols - 2].Background.custom = true;

    Row_p->Glyphs_p[cols - 3].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 3] = true;
    Row_p->Glyphs_p[cols - 3].Background.custom = true;

    Row_p->Glyphs_p[cols - 4].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 4] = true;
    Row_p->Glyphs_p[cols - 4].Background.custom = true;

    Row_p->Glyphs_p[cols - 5].codepoint = 0x25bc;
    Row_p->Glyphs_p[cols - 5].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 5] = true;
    Row_p->Glyphs_p[cols - 5].Background.custom = true;

    Row_p->Glyphs_p[cols - 6].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 6] = true;
    Row_p->Glyphs_p[cols - 6].Background.custom = true;

    Row_p->Glyphs_p[cols - 7].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 7] = true;
    Row_p->Glyphs_p[cols - 7].Background.custom = true;

    Row_p->Glyphs_p[cols - 8].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->Glyphs_p[cols - 8].Attributes.bold = true;
    Row_p->Glyphs_p[cols - 8].codepoint = 0x25b2;
    Row_p->update_p[cols - 8] = true;
    Row_p->Glyphs_p[cols - 8].Background.custom = true;

    Row_p->Glyphs_p[cols - 9].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 9] = true;
    Row_p->Glyphs_p[cols - 9].Background.custom = true;

if (TTY_p->Config.Sidebar.on == false && TTY_p->Config.clean == false) {

    Row_p->Glyphs_p[cols - 10].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 10] = true;
    Row_p->Glyphs_p[cols - 10].Background.custom = true;

    Row_p->Glyphs_p[cols - 11].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->Glyphs_p[cols - 11].Attributes.bold = false;
    Row_p->Glyphs_p[cols - 11].codepoint = '|';
    Row_p->update_p[cols - 11] = true;
    Row_p->Glyphs_p[cols - 11].Background.custom = true;

    Row_p->Glyphs_p[cols - 12].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 12] = true;
    Row_p->Glyphs_p[cols - 12].Background.custom = true;


	cols -= 3;

    Row_p->Glyphs_p[cols - 10].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 10] = true;
    Row_p->Glyphs_p[cols - 10].Background.custom = true;

    Row_p->Glyphs_p[cols - 11].codepoint = '-';
    Row_p->Glyphs_p[cols - 11].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 11] = true;
    Row_p->Glyphs_p[cols - 11].Background.custom = true;
    Row_p->Glyphs_p[cols - 11].Attributes.bold = true;

    Row_p->Glyphs_p[cols - 12].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 12] = true;
    Row_p->Glyphs_p[cols - 12].Background.custom = true;


    Row_p->Glyphs_p[cols - 13].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 13] = true;
    Row_p->Glyphs_p[cols - 13].Background.custom = true;

    Row_p->Glyphs_p[cols - 14].codepoint = '+';
    Row_p->Glyphs_p[cols - 14].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 14] = true;
    Row_p->Glyphs_p[cols - 14].Background.custom = true;
    Row_p->Glyphs_p[cols - 14].Attributes.bold = true;

    Row_p->Glyphs_p[cols - 15].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 15] = true;
    Row_p->Glyphs_p[cols - 15].Background.custom = true;



    Row_p->Glyphs_p[cols - 16].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 16] = true;
    Row_p->Glyphs_p[cols - 16].Background.custom = true;

    Row_p->Glyphs_p[cols - 17].Attributes.bold = false;
    Row_p->Glyphs_p[cols - 17].codepoint = '?';
    Row_p->Glyphs_p[cols - 17].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 17] = true;
    Row_p->Glyphs_p[cols - 17].Background.custom = true;

    Row_p->Glyphs_p[cols - 18].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 18] = true;
    Row_p->Glyphs_p[cols - 18].Background.custom = true;



    Row_p->Glyphs_p[cols - 19].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 19] = true;
    Row_p->Glyphs_p[cols - 19].Background.custom = true;

    Row_p->Glyphs_p[cols - 20].codepoint = 0x2261;
    Row_p->Glyphs_p[cols - 20].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
    Row_p->update_p[cols - 20] = true;
    Row_p->Glyphs_p[cols - 20].Background.custom = true;
    Row_p->Glyphs_p[cols - 20].Attributes.bold = true;


//    Row_p->Glyphs_p[cols - 21].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 21] = true;
//    Row_p->Glyphs_p[cols - 21].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 22].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 22] = true;
//    Row_p->Glyphs_p[cols - 22].Background.custom = true;
//
//
//
//    Row_p->Glyphs_p[cols - 23].codepoint = 0x2261;
//    Row_p->Glyphs_p[cols - 23].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 23] = true;
//    Row_p->Glyphs_p[cols - 23].Background.custom = true;
//
//
//
//    Row_p->Glyphs_p[cols - 24].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 24] = true;
//    Row_p->Glyphs_p[cols - 24].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 25].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 25] = true;
//    Row_p->Glyphs_p[cols - 25].Background.custom = true;
//
//
//
//    Row_p->Glyphs_p[cols - 26].codepoint = 0x2261;
//    Row_p->Glyphs_p[cols - 26].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 26] = true;
//    Row_p->Glyphs_p[cols - 26].Background.custom = true;
//    Row_p->Glyphs_p[cols - 26].Attributes.bold = true;


//    Row_p->Glyphs_p[cols - 21].codepoint = 'l';
//    Row_p->Glyphs_p[cols - 21].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 21] = true;
//    Row_p->Glyphs_p[cols - 21].Background.custom = true;
//
//
//    Row_p->Glyphs_p[cols - 22].codepoint = 'e';
//    Row_p->Glyphs_p[cols - 22].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 22] = true;
//    Row_p->Glyphs_p[cols - 22].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 23].codepoint = 'H';
//    Row_p->Glyphs_p[cols - 23].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 23] = true;
//    Row_p->Glyphs_p[cols - 23].Background.custom = true;
//
//
//
//    Row_p->Glyphs_p[cols - 24].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 24] = true;
//    Row_p->Glyphs_p[cols - 24].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 25].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 25] = true;
//    Row_p->Glyphs_p[cols - 25].Background.custom = true;
//
//
//
//
//    Row_p->Glyphs_p[cols - 26].codepoint = 's';
//    Row_p->Glyphs_p[cols - 26].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 26] = true;
//    Row_p->Glyphs_p[cols - 26].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 27].codepoint = 'g';
//    Row_p->Glyphs_p[cols - 27].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 27] = true;
//    Row_p->Glyphs_p[cols - 27].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 28].codepoint = 'n';
//    Row_p->Glyphs_p[cols - 28].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 28] = true;
//    Row_p->Glyphs_p[cols - 28].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 29].codepoint = 'i';
//    Row_p->Glyphs_p[cols - 29].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 29] = true;
//    Row_p->Glyphs_p[cols - 29].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 30].codepoint = 't';
//    Row_p->Glyphs_p[cols - 30].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 30] = true;
//    Row_p->Glyphs_p[cols - 30].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 31].codepoint = 't';
//    Row_p->Glyphs_p[cols - 31].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 31] = true;
//    Row_p->Glyphs_p[cols - 31].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 32].codepoint = 'e';
//    Row_p->Glyphs_p[cols - 32].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 32] = true;
//    Row_p->Glyphs_p[cols - 32].Background.custom = true;
//
//    Row_p->Glyphs_p[cols - 33].codepoint = 'S';
//    Row_p->Glyphs_p[cols - 33].mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_ACCENT_BACKGROUND_2;
//    Row_p->update_p[cols - 33] = true;
//    Row_p->Glyphs_p[cols - 33].Background.custom = true;



}

    int count = TTY_p->Config.Sidebar.on ? 4 : 2;
    for (int i = 0; i < TTY_p->Windows.size; ++i) {

        tk_core_MacroWindow *Window_p = TTY_p->Windows.pp[i];
        Row_p->Glyphs_p[count].codepoint = i + '1';

        Row_p->update_p[count - 2] = true;
        Row_p->update_p[count - 1] = true;
        Row_p->update_p[count] = true;
        Row_p->update_p[count + 1] = true;
        Row_p->update_p[count + 2] = true;
 
        TK_CORE_MARK_E background = TTY_p->Titlebar.hover >= count - 2 && TTY_p->Titlebar.hover <= count + 2 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

        if (Window_p == TTY_p->Window_p) {
            background = TK_CORE_MARK_ACCENT_BACKGROUND;
        }
        Glyphs_p[count - 2].codepoint = 0;
        Glyphs_p[count - 2].Attributes.reverse = false;
        Glyphs_p[count - 2].mark |= background;
        Glyphs_p[count - 2].Background.custom = true;
 
        Glyphs_p[count - 1].codepoint = 0;
        Glyphs_p[count - 1].Attributes.reverse = false;
        Glyphs_p[count - 1].mark |= background;
        Glyphs_p[count - 1].Background.custom = true;
 
        Glyphs_p[count].Attributes.reverse = false;
        Glyphs_p[count].Attributes.bold = true;
        Glyphs_p[count].Background.custom = true;
        Glyphs_p[count].mark = TK_CORE_MARK_ACCENT | background;
 
        Glyphs_p[count + 1].Background.custom = true;
        Glyphs_p[count + 1].mark = TK_CORE_MARK_ACCENT | background;
        Glyphs_p[count + 1].codepoint = 0;
        Glyphs_p[count + 1].Attributes.reverse = false;
 
        Glyphs_p[count + 2].Background.custom = true;
        Glyphs_p[count + 2].mark = TK_CORE_MARK_ACCENT | background;
        Glyphs_p[count + 2].codepoint = 0;
        Glyphs_p[count + 2].Attributes.reverse = false;

        count += 5;
    }

    Row_p->update_p[count - 2] = true;
    Row_p->update_p[count - 1] = true;
    Row_p->update_p[count] = true;
    Row_p->update_p[count + 1] = true;
    Row_p->update_p[count + 2] = true;
 
    TK_CORE_MARK_E bla = TTY_p->Titlebar.hover > count - 2 && TTY_p->Titlebar.hover < count + 2 ? TK_CORE_MARK_ACCENT_BACKGROUND : TK_CORE_MARK_ACCENT_BACKGROUND_2;

    Glyphs_p[count - 2].codepoint = 0;
    Glyphs_p[count - 2].Attributes.reverse = false;
    Glyphs_p[count - 2].mark |= bla;
    Glyphs_p[count - 2].Background.custom = true;
 
    Glyphs_p[count - 1].codepoint = 0;
    Glyphs_p[count - 1].Attributes.reverse = false;
    Glyphs_p[count - 1].mark |= bla;
    Glyphs_p[count - 1].Background.custom = true;
 
    Glyphs_p[count].codepoint = '+';
    Glyphs_p[count].Attributes.reverse = false;
    Glyphs_p[count].Attributes.bold = true;
    Glyphs_p[count].Background.custom = true;
    Glyphs_p[count].mark = TK_CORE_MARK_ACCENT | bla;
 
    Glyphs_p[count + 1].Background.custom = true;
    Glyphs_p[count + 1].mark = bla;
    Glyphs_p[count + 1].codepoint = 0;
    Glyphs_p[count + 1].Attributes.reverse = false;
 
    Glyphs_p[count + 2].Background.custom = true;
    Glyphs_p[count + 2].mark = bla;
    Glyphs_p[count + 2].codepoint = 0;
    Glyphs_p[count + 2].Attributes.reverse = false;
 
    TTY_p->Titlebar.count = count;

    if (TTY_p->Config.Sidebar.on) {
        Row_p->update_p[0] = 1;
        Row_p->update_p[1] = 1;
        Row_p->Glyphs_p[0].codepoint = 0x2261;
        Row_p->Glyphs_p[0].Attributes.bold = true;
        if (TTY_p->Sidebar.hover == -1) {
            Row_p->Glyphs_p[0].mark = TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_ACCENT | TK_CORE_MARK_SIDEBAR;
            Row_p->Glyphs_p[1].mark = TK_CORE_MARK_ACCENT_BACKGROUND | TK_CORE_MARK_SIDEBAR;
        } else {
            Row_p->Glyphs_p[0].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2 | TK_CORE_MARK_ACCENT | TK_CORE_MARK_SIDEBAR;
            Row_p->Glyphs_p[1].mark = TK_CORE_MARK_ACCENT_BACKGROUND_2 | TK_CORE_MARK_SIDEBAR;
        }
        Row_p->Glyphs_p[0].Background.custom = true;
        Row_p->Glyphs_p[1].Background.custom = true;
    }

//char *p = "CPU 45% RAM 86% UPTIME 4D12H34M";
//for (int i = 0; i < strlen(p); ++i) {
//Glyphs_p[count + 4 + i].codepoint = p[i];
//Glyphs_p[count + 4 + i].mark = TK_CORE_MARK_ACCENT_2 | TK_CORE_MARK_ACCENT_BACKGROUND_3;
//}
    return TK_CORE_SUCCESS;
}
