#ifndef TK_GFX_GRID_H
#define TK_GFX_GRID_H

// INCLUDES ========================================================================================

#include "../Common/Includes.h"
#include "../Common/Config.h"

#include "nh-core/Util/Time.h"
#include "nh-gfx/Fonts/Text.h"

#include "../../tk-core/Common/Config.h"

// STRUCTS =========================================================================================

typedef struct tk_gfx_Box {
    bool accent;
    nh_api_PixelPosition UpperLeft;
    nh_api_PixelPosition LowerRight;
    float innerVertices_p[18];
    float outerVertices_p[18];
} tk_gfx_Box;

typedef struct tk_gfx_TileUpdate {
    tk_core_Glyph Glyph;
    int row;
    int col;
    bool cursor;
} tk_gfx_TileUpdate;

typedef struct tk_gfx_TileForeground {
    float vertices_p[24];
} tk_gfx_TileForeground;

typedef struct tk_gfx_TileBackground {
    float vertices_p[12];
} tk_gfx_TileBackground;

typedef struct tk_gfx_TileOverlay {
    float vertices_p[24];
} tk_gfx_TileOverlay;

typedef struct tk_gfx_Tile {
    tk_gfx_TileForeground Foreground;
    tk_gfx_TileBackground Background;
    tk_gfx_TileOverlay Overlay;
    tk_core_Glyph Glyph;
    bool dirty;
} tk_gfx_Tile;

typedef struct tk_gfx_Grid {
    nh_core_List Rows;
    nh_api_PixelSize TileSize;
    nh_api_PixelSize Size;
    int cols;
    int rows;
    int xOffset, yOffset;
    tk_gfx_Tile *Cursor_p;
    tk_gfx_TileUpdate **Updates_pp;
    bool **updates_pp;
    int borderPixel;
    int borderPixelLeft;
    int borderPixelRight;
    int borderPixelTop;
    int borderPixelBottom;
    int separatorPixel;
    float verticalGap;
} tk_gfx_Grid;

// FUNCTIONS =======================================================================================

tk_gfx_Tile *tk_gfx_getTile(
    tk_gfx_Grid *Grid_p, int row, int col
);

tk_gfx_Tile *tk_gfx_getTileUnsafe(
    tk_gfx_Grid *Grid_p, int row, int col
);

TK_API_RESULT tk_gfx_initGrid(
    tk_gfx_Grid *Grid_p
);

TK_API_RESULT tk_gfx_freeGrid(
    tk_gfx_Grid *Grid_p
);

TK_API_RESULT tk_gfx_updateBackdropGrid(
    tk_gfx_Config *Config_p, tk_core_Config *CoreConfig_p, tk_gfx_Grid *BackdropGrid_p,
    void *state_p, nh_gfx_Text *Text_p, tk_gfx_Grid *Grid_p
);

TK_API_RESULT tk_gfx_updateGrid(
    tk_gfx_Config *Config_p, tk_core_Config *CoreConfig_p, tk_gfx_Grid *Grid_p, void *state_p,
    nh_gfx_Text *Text_p
);

bool tk_gfx_compareBackgroundAttributes(
    tk_core_Glyph *Glyph1_p, tk_core_Glyph *Glyph2_p
);

bool tk_gfx_compareForegroundAttributes(
    tk_core_Glyph *Glyph1_p, tk_core_Glyph *Glyph2_p
);

TK_API_RESULT tk_gfx_updateTile(
    tk_gfx_Grid *Grid_p, void *state_p, tk_gfx_TileUpdate *Update_p, bool *update_p, int fontSize
);

#endif 
