#ifndef TK_GFX_OPENGL_DIM_H
#define TK_GFX_OPENGL_DIM_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

#include "../Common/Includes.h"

#include "nh-gfx/OpenGL/CommandBuffer.h"
#include "nh-gfx/OpenGL/Commands.h"

#include "nh-gfx/Fonts/FontManager.h"

#include <stdint.h>

typedef struct tk_gfx_Graphics tk_gfx_Graphics;

#endif

/** @addtogroup lib_nhterminal_structs
 *  @{
 */

    typedef struct tk_gfx_OpenGLDim {
        nh_gfx_OpenGLCommand *VertexShader_p;
        nh_gfx_OpenGLCommand *FragmentShader_p;
        nh_gfx_OpenGLCommand *Program_p;
        nh_gfx_OpenGLData *VertexArray_p;
        nh_gfx_OpenGLData *VerticesBuffer_p;
    } tk_gfx_OpenGLDim;

/** @} */

/** @addtogroup lib_nhterminal_functions
 *  @{
 */

    TK_API_RESULT tk_gfx_updateOpenGLDim(
        void *state_p, void *data_p
    );

    TK_API_RESULT tk_gfx_initOpenGLDim(
        tk_gfx_OpenGLDim *Dim_p
    );

    TK_API_RESULT tk_gfx_freeOpenGLDim(
        tk_gfx_OpenGLDim *Dim_p
    );

/** @} */

#endif 
