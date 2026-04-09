#ifndef TK_GFX_TERMINAL_VERTICES_H
#define TK_GFX_TERMINAL_VERTICES_H

// INCLUDES ========================================================================================

#include "Grid.h"
#include "Graphics.h"

// FUNCTIONS =======================================================================================

TK_API_RESULT tk_gfx_getBackgroundVertices(
    tk_gfx_GraphicsState *State_p, tk_gfx_Grid *Grid_p, tk_core_Glyph *Glyph_p, int col,
    int row, float vertices_p[12], int fontSize
);

TK_API_RESULT tk_gfx_getBoxVertices(
    tk_gfx_GraphicsState *State_p, tk_gfx_Grid *Grid_p, tk_gfx_Box *Box_p,
    bool inner, int fontSize
);

TK_API_RESULT tk_gfx_getOutlineVertices(
    tk_gfx_GraphicsState *State_p, tk_gfx_Grid *Grid_p, tk_gfx_Box *Box_p,
    bool inner, int fontSize
);

TK_API_RESULT tk_gfx_getForegroundVertices(
    tk_gfx_GraphicsState *State_p, tk_gfx_Grid *Grid_p, tk_core_Glyph *Glyph_p, int col,
    int row, float *vertices_p, int fontSize
);

TK_API_RESULT tk_gfx_getOverlayVertices(
    tk_gfx_GraphicsState *State_p, tk_gfx_Grid *Grid_p, tk_core_Glyph *Glyph_p, int col,
    int row, float *vertices_p, int fontSize
);

#endif // TK_GFX_TERMINAL_VERTICES_H 
