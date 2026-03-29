#ifndef TK_GFX_OPENGL_BOXES_H
#define TK_GFX_OPENGL_BOXES_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

#include "../Common/Includes.h"

#include "nh-gfx/OpenGL/CommandBuffer.h"
#include "nh-gfx/OpenGL/Commands.h"

#endif

/** @addtogroup lib_nhterminal_structs
 *  @{
 */

    typedef struct tk_gfx_OpenGLBoxes {
        nh_gfx_OpenGLData *VertexArray_p;
        nh_gfx_OpenGLData *VerticesBuffer_p;
        nh_gfx_OpenGLData *ColorBuffer_p;
        nh_gfx_OpenGLCommand *BufferData_p;
    } tk_gfx_OpenGLBoxes;

/** @} */

/** @addtogroup lib_nhterminal_functions
 *  @{
 */

    TK_API_RESULT tk_gfx_updateOpenGLBoxes(
        void *state_p, void *data_p
    );

    TK_API_RESULT tk_gfx_initOpenGLBoxes(
        tk_gfx_OpenGLBoxes *Boxes_p
    );

    TK_API_RESULT tk_gfx_freeOpenGLBoxes(
        tk_gfx_OpenGLBoxes *Boxes_p
    );

/** @} */

#endif 
