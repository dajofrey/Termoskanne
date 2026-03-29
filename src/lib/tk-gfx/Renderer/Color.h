#ifndef TK_GFX_TERMINAL_COLOR_H
#define TK_GFX_TERMINAL_COLOR_H

// INCLUDES ========================================================================================
 
#include "Grid.h"
#include "Graphics.h"

#if defined(__unix__)
    #include "../Vulkan/Foreground.h"
#endif

#include "../OpenGL/Foreground.h"
#include "../OpenGL/Background.h"
#include "../OpenGL/Boxes.h"

#include "nh-core/Util/HashMap.h"
#include "nh-gfx/Base/Viewport.h"

// FUNCTIONS =======================================================================================

tk_core_Color tk_gfx_getGlyphColor(
    tk_gfx_Config *Config_p, tk_gfx_GraphicsState *State_p, tk_core_Glyph *Glyph_p,
    unsigned int target, int col, int row, tk_gfx_Grid *Grid_p
);

tk_core_Color tk_gfx_getOverlayColor(
    tk_gfx_Config *Config_p, tk_gfx_GraphicsState *State_p, tk_core_Glyph *Glyph_p,
    int col, int row, tk_gfx_Grid *Grid_p
);

tk_core_Color tk_gfx_getGradientColor(
    tk_gfx_GraphicsGradient *Gradient_p, tk_core_Color *Colors_p, int colors
);

#endif 
