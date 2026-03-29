#ifndef TK_GFX_OPENGL_RENDER_H
#define TK_GFX_OPENGL_RENDER_H

#include "../Renderer/Graphics.h"
#include "../Common/Includes.h"

TK_API_RESULT tk_gfx_renderUsingOpenGL(
    tk_gfx_Config *Config_p, tk_gfx_Graphics *Graphics_p, tk_gfx_Grid *Grid_p,
    tk_gfx_Grid *BackdropGrid_p, unsigned int sidebar
);

#endif
