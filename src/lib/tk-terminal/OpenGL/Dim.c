// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES =======================================================================================

#include "Dim.h"

#include "../Terminal/Graphics.h"
#include "../Common/Macros.h"

#include "nh-gfx/Base/Viewport.h"
#include "nh-gfx/Common/Macros.h"

#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>
#include <float.h>

// FUNCTIONS =======================================================================================

static TK_TERMINAL_RESULT tk_terminal_initOpenGLDimProgram(
    tk_terminal_OpenGLDim *Dim_p, nh_gfx_OpenGLCommandBuffer *CommandBuffer_p)
{
    static const char* vsSource_p =
        "#version 450\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec4 in_color;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    color = in_color;\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n";

    static const char* fsSource_p =
        "#version 450\n"
        "in vec4 color;\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "    fragColor = color;\n"
        "}\n";

    Dim_p->VertexShader_p =
        nh_gfx_disableOpenGLCommandAutoFree(
            nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCreateShader", nh_gfx_glint(NULL, GL_VERTEX_SHADER)));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glShaderSource",
        &Dim_p->VertexShader_p->Result, nh_gfx_gluint(NULL, 1),
        nh_gfx_glchar(NULL, NULL, 0, (GLchar**)&vsSource_p),
        nh_gfx_glpointer(NULL, NULL));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCompileShader", &Dim_p->VertexShader_p->Result);

    Dim_p->FragmentShader_p =
        nh_gfx_disableOpenGLCommandAutoFree(
            nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCreateShader", nh_gfx_glint(NULL, GL_FRAGMENT_SHADER)));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glShaderSource",
        &Dim_p->FragmentShader_p->Result, nh_gfx_gluint(NULL, 1),
        nh_gfx_glchar(NULL, NULL, 0, (GLchar**)&fsSource_p),
        nh_gfx_glpointer(NULL, NULL));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCompileShader", &Dim_p->FragmentShader_p->Result);

    Dim_p->Program_p = 
        nh_gfx_disableOpenGLCommandAutoFree(nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCreateProgram"));

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glAttachShader", &Dim_p->Program_p->Result,
        &Dim_p->VertexShader_p->Result);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glAttachShader", &Dim_p->Program_p->Result,
        &Dim_p->FragmentShader_p->Result);

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glLinkProgram", &Dim_p->Program_p->Result);

    return TK_TERMINAL_SUCCESS;
}

static TK_TERMINAL_RESULT tk_terminal_initOpenGLDimVertices(
    tk_terminal_OpenGLDim *Dim_p, nh_gfx_OpenGLCommandBuffer *CommandBuffer_p)
{
    Dim_p->VertexArray_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenVertexArrays", nh_gfx_gluint(NULL, 1),
        Dim_p->VertexArray_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindVertexArray", Dim_p->VertexArray_p);

    Dim_p->VerticesBuffer_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenBuffers", nh_gfx_gluint(NULL, 1), 
        Dim_p->VerticesBuffer_p);

    return TK_TERMINAL_SUCCESS;
}

static TK_TERMINAL_RESULT tk_terminal_updateOpenGLDimVertices(
    tk_terminal_OpenGLDim *Dim_p, nh_gfx_OpenGLCommandBuffer *CommandBuffer_p, nh_core_Array *Vertices_p)
{
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindVertexArray", Dim_p->VertexArray_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), Dim_p->VerticesBuffer_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBufferData", 
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), 
        nh_gfx_glsizeiptr(NULL, Vertices_p->length * sizeof(GLfloat)),
        nh_gfx_glubyte(NULL, Vertices_p->p, Vertices_p->length * sizeof(GLfloat)),
        nh_gfx_glenum(NULL, GL_STATIC_DRAW));

    // Enable and describe attribute 0: position (vec3)
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glEnableVertexAttribArray", nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glVertexAttribPointer",
        nh_gfx_gluint(NULL, 0), nh_gfx_gluint(NULL, 3), nh_gfx_glenum(NULL, GL_FLOAT),
        nh_gfx_glboolean(NULL, GL_FALSE), nh_gfx_glsizei(NULL, sizeof(float) * 7),
        nh_gfx_glpointer(NULL, NULL));
    
    // Enable and describe attribute 1: color (vec4)
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glEnableVertexAttribArray", nh_gfx_gluint(NULL, 1));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glVertexAttribPointer",
        nh_gfx_gluint(NULL, 1), nh_gfx_gluint(NULL, 4), nh_gfx_glenum(NULL, GL_FLOAT),
        nh_gfx_glboolean(NULL, GL_FALSE), nh_gfx_glsizei(NULL, sizeof(float) * 7),
        nh_gfx_glpointer(NULL, (void*)(sizeof(float) * 3)));

    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_updateOpenGLDim(
    void *state_p, void *data_p)
{
    tk_terminal_GraphicsState *State_p = state_p;
    tk_terminal_Dim *Dim_p = data_p;

    if (Dim_p->Action.init) {
        tk_terminal_initOpenGLDimProgram(&Dim_p->OpenGL, 
            State_p->Viewport_p->OpenGL.CommandBuffer_p);
        tk_terminal_initOpenGLDimVertices(&Dim_p->OpenGL, 
            State_p->Viewport_p->OpenGL.CommandBuffer_p);
        Dim_p->Action.init = false;
    }

    tk_terminal_updateOpenGLDimVertices(&Dim_p->OpenGL,
        State_p->Viewport_p->OpenGL.CommandBuffer_p, &Dim_p->Vertices);

    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_initOpenGLDim(
    tk_terminal_OpenGLDim *Dim_p)
{
    memset(Dim_p, 0, sizeof(tk_terminal_OpenGLDim));
 
    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_freeOpenGLDim(
    tk_terminal_OpenGLDim *Dim_p)
{
    nh_gfx_freeOpenGLCommand(Dim_p->VertexShader_p);
    nh_gfx_freeOpenGLCommand(Dim_p->FragmentShader_p);
    nh_gfx_freeOpenGLCommand(Dim_p->Program_p);
    
    nh_gfx_freeOpenGLData(Dim_p->VertexArray_p);
    nh_gfx_freeOpenGLData(Dim_p->VerticesBuffer_p);

    return TK_TERMINAL_SUCCESS;
}
