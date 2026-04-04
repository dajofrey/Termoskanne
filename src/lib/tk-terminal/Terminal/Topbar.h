#ifndef TK_TERMINAL_TOPBAR_H
#define TK_TERMINAL_TOPBAR_H

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

#include "nh-core/Util/Array.h"
#include "nh-core/Util/Time.h"
#include "nh-encoding/Encodings/UTF32.h"

#include "../Common/Includes.h"
#include "../Common/Config.h"

#include "../../tk-core/Core/Tiling.h"
#include "../../tk-core/Core/Program.h"

typedef struct tk_core_MicroWindow tk_core_MicroWindow;

typedef struct tk_terminal_Topbar {
    int cursorX;
    int quitPosition;
    int tilingPosition;
    bool hasFocus;
    bool refresh;
    bool quitHover;
    bool tilingHover;
} tk_terminal_Topbar;

tk_terminal_Topbar tk_terminal_initTopbar(
);

TK_API_RESULT tk_terminal_handleTopbarInput(
    tk_core_Tile *Tile_p, nh_api_WSIEvent Event 
);

TK_API_RESULT tk_terminal_drawTopbar(
    tk_core_Tile *Tile_p, tk_core_Glyph *Glyphs_p, int cols, int row, bool standardIO
);

#endif 
