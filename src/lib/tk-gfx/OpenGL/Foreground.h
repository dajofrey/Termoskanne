#ifndef TK_GFX_OPENGL_FOREGROUND_H
#define TK_GFX_OPENGL_FOREGROUND_H

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

    typedef struct tk_gfx_OpenGLForeground {
        nh_gfx_OpenGLCommand *VertexShader_p;
        nh_gfx_OpenGLCommand *FragmentShader_p;
        nh_gfx_OpenGLCommand *Program_p;
        nh_gfx_OpenGLCommand *VertexShader2_p;
        nh_gfx_OpenGLCommand *FragmentShader2_p;
        nh_gfx_OpenGLCommand *Program2_p;
        nh_gfx_OpenGLData *Texture_p;
        nh_gfx_OpenGLData *BoldTexture_p;
        nh_gfx_OpenGLData *VertexArray_p;
        nh_gfx_OpenGLData *VertexArray2_p;
        nh_gfx_OpenGLData *IndicesBuffer_p;
        nh_gfx_OpenGLData *IndicesBuffer2_p;
        nh_gfx_OpenGLData *VerticesBuffer_p;
        nh_gfx_OpenGLData *VerticesBuffer2_p;
        nh_gfx_OpenGLData *ColorBuffer_p;
        nh_gfx_OpenGLData *ColorBuffer2_p;
        nh_gfx_OpenGLCommand *BufferData_p;
        nh_gfx_OpenGLCommand *BufferElevatedData_p;
        nh_gfx_OpenGLCommand *GetUniformLocationTexture_p;
    } tk_gfx_OpenGLForeground;

/** @} */

/** @addtogroup lib_nhterminal_functions
 *  @{
 */

    TK_API_RESULT tk_gfx_updateOpenGLForeground(
        void *Config_p, void *state_p, void *data_p
    );

    TK_API_RESULT tk_gfx_initOpenGLForeground(
        tk_gfx_OpenGLForeground *Text_p
    );

    TK_API_RESULT tk_gfx_freeOpenGLForeground(
        tk_gfx_OpenGLForeground *Foreground_p
    );

/** @} */

#endif 
