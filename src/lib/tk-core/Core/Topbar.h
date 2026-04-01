#ifndef TK_CORE_TOPBAR_H
#define TK_CORE_TOPBAR_H

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

#include "Tiling.h"
#include "Program.h"

#include "nh-core/Util/Array.h"
#include "nh-core/Util/Time.h"
#include "nh-encoding/Encodings/UTF32.h"

#include "../Common/Includes.h"
#include "../Common/Config.h"

typedef struct tk_core_MicroWindow tk_core_MicroWindow;

/** @addtogroup lib_nhtty_structs
 *  @{
 */

    typedef struct tk_core_TopbarSuggestions {
        nh_core_SystemTime Time;
	tk_core_Program *ProgramSuggestion_p;
	char *CommandSuggestion_p;
	double interval;
	bool update;
	int commandIndex;
	int programIndex;
    } tk_core_TopbarSuggestions;

    typedef struct tk_core_Topbar {
        int cursorX;
        int quitPosition;
        int tilingPosition;
        bool hasFocus;
	bool refresh;
        bool quitHover;
        bool tilingHover;
    } tk_core_Topbar;

/** @} */

/** @addtogroup lib_nhtty_functions
 *  @{
 */

    tk_core_Topbar tk_core_initTopbar(
    );

    TK_API_RESULT tk_core_freeTopbar(
        tk_core_Topbar *Topbar_p
    );

    TK_API_RESULT tk_core_handleTopbarInput(
        tk_core_Tile *Tile_p, nh_api_WSIEvent Event 
    );

    TK_API_RESULT tk_core_drawTopbarRow(
        tk_core_Tile *Tile_p, tk_core_Glyph *Glyphs_p, int cols, int row, bool standardIO
    );

/** @} */

#endif 
