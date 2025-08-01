// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES =======================================================================================

#include "Foreground.h"

#include "../Terminal/Graphics.h"

#include "../Common/Macros.h"
#include "../Common/Config.h"

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

static TK_TERMINAL_RESULT tk_terminal_initOpenGLForegroundPrograms(
    tk_terminal_OpenGLForeground *Foreground_p, nh_gfx_OpenGLCommandBuffer *CommandBuffer_p)
{
    static const char* vsSource_p =
        "#version 450\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec2 uv;\n"
        "layout(location=2) in vec4 in_color;\n"
        "out vec2 texcoord;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    texcoord = uv.xy;\n"
        "    color = in_color;\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n";

    static const char* fsSource_p =
        "#version 450\n"
        "#if __VERSION__ < 130\n"
        "#define TEXTURE2D texture2D\n"
        "#else\n"
        "#define TEXTURE2D texture\n"
        "#endif\n"
        "uniform sampler2D u_texture;\n"
        "in vec2 texcoord;\n"
        "in vec4 color;\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "    float dist = TEXTURE2D(u_texture, texcoord.st).r;\n"
        "    float width = fwidth(dist);\n"
        "    float alpha = smoothstep(0.5-width, 0.5+width, dist);\n"
        "    fragColor = vec4(color.rgb, alpha*color.a);\n"
        "}\n";

    Foreground_p->VertexShader_p =
        nh_gfx_disableOpenGLCommandAutoFree(
            nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCreateShader", nh_gfx_glint(NULL, GL_VERTEX_SHADER)));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glShaderSource",
        &Foreground_p->VertexShader_p->Result, nh_gfx_gluint(NULL, 1),
        nh_gfx_glchar(NULL, NULL, 0, (GLchar**)&vsSource_p),
        nh_gfx_glpointer(NULL, NULL));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCompileShader", &Foreground_p->VertexShader_p->Result);

    Foreground_p->FragmentShader_p =
        nh_gfx_disableOpenGLCommandAutoFree(
            nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCreateShader", nh_gfx_glint(NULL, GL_FRAGMENT_SHADER)));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glShaderSource",
        &Foreground_p->FragmentShader_p->Result, nh_gfx_gluint(NULL, 1),
        nh_gfx_glchar(NULL, NULL, 0, (GLchar**)&fsSource_p),
        nh_gfx_glpointer(NULL, NULL));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCompileShader", &Foreground_p->FragmentShader_p->Result);

    Foreground_p->Program_p = 
        nh_gfx_disableOpenGLCommandAutoFree(nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCreateProgram"));

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glAttachShader", &Foreground_p->Program_p->Result,
        &Foreground_p->VertexShader_p->Result);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glAttachShader", &Foreground_p->Program_p->Result,
        &Foreground_p->FragmentShader_p->Result);

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glLinkProgram", &Foreground_p->Program_p->Result);

    static GLchar *textureName_p = "u_texture";
    Foreground_p->GetUniformLocationTexture_p =
        nh_gfx_disableOpenGLCommandAutoFree(nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGetUniformLocation",
            &Foreground_p->Program_p->Result, nh_gfx_glchar(NULL, NULL, 0, &textureName_p)));

    static const char* vsSource2_p =
        "#version 450\n"
        "layout(location=0) in vec3 position;\n"
        "layout(location=1) in vec4 in_color;\n"
        "out vec4 color;\n"
        "void main() {\n"
        "    color = in_color;\n"
        "    gl_Position = vec4(position, 1.0);\n"
        "}\n";

    static const char* fsSource2_p =
        "#version 450\n"
        "in vec4 color;\n"
        "out vec4 fragColor;\n"
        "void main() {\n"
        "    fragColor = vec4(color.rgb, color.a);\n"
        "}\n";

    Foreground_p->VertexShader2_p =
        nh_gfx_disableOpenGLCommandAutoFree(
            nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCreateShader", nh_gfx_glint(NULL, GL_VERTEX_SHADER)));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glShaderSource",
        &Foreground_p->VertexShader2_p->Result, nh_gfx_gluint(NULL, 1),
        nh_gfx_glchar(NULL, NULL, 0, (GLchar**)&vsSource2_p),
        nh_gfx_glpointer(NULL, NULL));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCompileShader", &Foreground_p->VertexShader2_p->Result);

    Foreground_p->FragmentShader2_p =
        nh_gfx_disableOpenGLCommandAutoFree(
            nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCreateShader", nh_gfx_glint(NULL, GL_FRAGMENT_SHADER)));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glShaderSource",
        &Foreground_p->FragmentShader2_p->Result, nh_gfx_gluint(NULL, 1),
        nh_gfx_glchar(NULL, NULL, 0, (GLchar**)&fsSource2_p),
        nh_gfx_glpointer(NULL, NULL));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCompileShader", &Foreground_p->FragmentShader2_p->Result);

    Foreground_p->Program2_p = 
        nh_gfx_disableOpenGLCommandAutoFree(nh_gfx_addOpenGLCommand(CommandBuffer_p, "glCreateProgram"));

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glAttachShader", &Foreground_p->Program2_p->Result,
        &Foreground_p->VertexShader2_p->Result);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glAttachShader", &Foreground_p->Program2_p->Result,
        &Foreground_p->FragmentShader2_p->Result);

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glLinkProgram", &Foreground_p->Program2_p->Result);

    return TK_TERMINAL_SUCCESS;
}

static TK_TERMINAL_RESULT tk_terminal_initOpenGLFontTextures(
    tk_terminal_OpenGLForeground *Foreground_p, nh_gfx_OpenGLCommandBuffer *CommandBuffer_p)
{
    Foreground_p->Texture_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    Foreground_p->BoldTexture_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));

    // regular
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenTextures", nh_gfx_glsizei(NULL, 1), Foreground_p->Texture_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glActiveTexture", nh_gfx_glenum(NULL, GL_TEXTURE0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindTexture",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), Foreground_p->Texture_p);

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glTexParameteri",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), nh_gfx_glenum(NULL, GL_TEXTURE_WRAP_S),
        nh_gfx_glenum(NULL, GL_CLAMP_TO_EDGE));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glTexParameteri",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), nh_gfx_glenum(NULL, GL_TEXTURE_WRAP_T),
        nh_gfx_glenum(NULL, GL_CLAMP_TO_EDGE));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glTexParameteri",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), nh_gfx_glenum(NULL, GL_TEXTURE_MAG_FILTER),
        nh_gfx_glenum(NULL, GL_LINEAR));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glTexParameteri",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), nh_gfx_glenum(NULL, GL_TEXTURE_MIN_FILTER),
        nh_gfx_glenum(NULL, GL_LINEAR));

    // bold
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenTextures", nh_gfx_glsizei(NULL, 1), Foreground_p->BoldTexture_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glActiveTexture", nh_gfx_glenum(NULL, GL_TEXTURE1));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindTexture",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), Foreground_p->BoldTexture_p);

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glTexParameteri",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), nh_gfx_glenum(NULL, GL_TEXTURE_WRAP_S),
        nh_gfx_glenum(NULL, GL_CLAMP_TO_EDGE));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glTexParameteri",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), nh_gfx_glenum(NULL, GL_TEXTURE_WRAP_T),
        nh_gfx_glenum(NULL, GL_CLAMP_TO_EDGE));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glTexParameteri",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), nh_gfx_glenum(NULL, GL_TEXTURE_MAG_FILTER),
        nh_gfx_glenum(NULL, GL_LINEAR));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glTexParameteri",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), nh_gfx_glenum(NULL, GL_TEXTURE_MIN_FILTER),
        nh_gfx_glenum(NULL, GL_LINEAR));

    return TK_TERMINAL_SUCCESS;
}

static TK_TERMINAL_RESULT tk_terminal_initOpenGLForegroundVertices(
    tk_terminal_OpenGLForeground *Foreground_p, nh_gfx_OpenGLCommandBuffer *CommandBuffer_p)
{
    Foreground_p->VertexArray_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenVertexArrays", nh_gfx_gluint(NULL, 1),
        Foreground_p->VertexArray_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindVertexArray", Foreground_p->VertexArray_p);

    Foreground_p->IndicesBuffer_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenBuffers", nh_gfx_gluint(NULL, 1), 
        Foreground_p->IndicesBuffer_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ELEMENT_ARRAY_BUFFER), Foreground_p->IndicesBuffer_p);

    Foreground_p->VerticesBuffer_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenBuffers", nh_gfx_gluint(NULL, 1), 
        Foreground_p->VerticesBuffer_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), Foreground_p->VerticesBuffer_p);

    Foreground_p->VertexArray2_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenVertexArrays", nh_gfx_gluint(NULL, 1),
        Foreground_p->VertexArray2_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindVertexArray", Foreground_p->VertexArray2_p);

    Foreground_p->IndicesBuffer2_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenBuffers", nh_gfx_gluint(NULL, 1), 
        Foreground_p->IndicesBuffer2_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ELEMENT_ARRAY_BUFFER), Foreground_p->IndicesBuffer2_p);

    Foreground_p->VerticesBuffer2_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenBuffers", nh_gfx_gluint(NULL, 1), 
        Foreground_p->VerticesBuffer2_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), Foreground_p->VerticesBuffer2_p);

    Foreground_p->ColorBuffer_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenBuffers", nh_gfx_gluint(NULL, 1), 
        Foreground_p->ColorBuffer_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), Foreground_p->ColorBuffer_p);

    Foreground_p->ColorBuffer2_p = nh_gfx_disableOpenGLDataAutoFree(nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glGenBuffers", nh_gfx_gluint(NULL, 1), 
        Foreground_p->ColorBuffer2_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), Foreground_p->ColorBuffer2_p);

    return TK_TERMINAL_SUCCESS;
}

static TK_TERMINAL_RESULT tk_terminal_updateOpenGLForegroundFont(
    tk_terminal_Config *Config_p, tk_terminal_GraphicsState *State_p,
    tk_terminal_GraphicsForeground *Foreground_p, nh_gfx_OpenGLCommandBuffer *CommandBuffer_p)
{
    // regular
    nh_gfx_FontInstance *FontInstance_p =
        nh_gfx_claimFontInstance(State_p->Fonts.pp[0], Config_p->fontSize);

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindTexture",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), Foreground_p->OpenGL.Texture_p);

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glTexImage2D",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), nh_gfx_glint(NULL, 0), nh_gfx_glint(NULL, GL_RED),
        nh_gfx_glsizei(NULL, FontInstance_p->Font_p->Atlas.width),
        nh_gfx_glsizei(NULL, FontInstance_p->Font_p->Atlas.height),
        nh_gfx_glint(NULL, 0), nh_gfx_glenum(NULL, GL_RED), nh_gfx_glenum(NULL, GL_UNSIGNED_BYTE),
        nh_gfx_glchar(NULL, NULL, 0, (GLchar**)&FontInstance_p->Font_p->Atlas.data_p));

    // bold
    nh_gfx_FontInstance *BoldFontInstance_p =
        nh_gfx_claimFontInstance(State_p->Fonts.pp[1], Config_p->fontSize);

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindTexture",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), Foreground_p->OpenGL.BoldTexture_p);

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glTexImage2D",
        nh_gfx_glenum(NULL, GL_TEXTURE_2D), nh_gfx_glint(NULL, 0), nh_gfx_glint(NULL, GL_RED),
        nh_gfx_glsizei(NULL, BoldFontInstance_p->Font_p->Atlas.width),
        nh_gfx_glsizei(NULL, BoldFontInstance_p->Font_p->Atlas.height),
        nh_gfx_glint(NULL, 0), nh_gfx_glenum(NULL, GL_RED), nh_gfx_glenum(NULL, GL_UNSIGNED_BYTE),
        nh_gfx_glchar(NULL, NULL, 0, (GLchar**)&BoldFontInstance_p->Font_p->Atlas.data_p));

    return TK_TERMINAL_SUCCESS;
}

static TK_TERMINAL_RESULT tk_terminal_updateOpenGLForegroundVertices(
    tk_terminal_GraphicsForeground *Foreground_p, nh_gfx_OpenGLCommandBuffer *CommandBuffer_p)
{
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindVertexArray", Foreground_p->OpenGL.VertexArray_p);

    // Indices.
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ELEMENT_ARRAY_BUFFER), Foreground_p->OpenGL.IndicesBuffer_p);
    Foreground_p->OpenGL.BufferData_p = nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBufferData",
        nh_gfx_glenum(NULL, GL_ELEMENT_ARRAY_BUFFER),
        nh_gfx_glsizeiptr(NULL, Foreground_p->Indices.length*sizeof(uint32_t)),
        nh_gfx_glubyte(NULL, Foreground_p->Indices.p, Foreground_p->Indices.length*sizeof(uint32_t)),
        nh_gfx_glenum(NULL, GL_STATIC_DRAW));

    // Vertices.
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), Foreground_p->OpenGL.VerticesBuffer_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBufferData", 
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), 
        nh_gfx_glsizeiptr(NULL, Foreground_p->Vertices.length*sizeof(GLfloat)),
        nh_gfx_glubyte(NULL, Foreground_p->Vertices.p, Foreground_p->Vertices.length*sizeof(GLfloat)),
        nh_gfx_glenum(NULL, GL_STATIC_DRAW));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glEnableVertexAttribArray", nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glVertexAttribPointer",
        nh_gfx_gluint(NULL, 0), nh_gfx_gluint(NULL, 3), nh_gfx_glenum(NULL, GL_FLOAT),
        nh_gfx_glboolean(NULL, GL_FALSE), nh_gfx_glsizei(NULL, sizeof(float)*5), 
        nh_gfx_glpointer(NULL, NULL));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glEnableVertexAttribArray", nh_gfx_gluint(NULL, 1));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glVertexAttribPointer",
        nh_gfx_gluint(NULL, 1), nh_gfx_gluint(NULL, 2), nh_gfx_glenum(NULL, GL_FLOAT),
        nh_gfx_glboolean(NULL, GL_FALSE), nh_gfx_glsizei(NULL, sizeof(float)*5),
        nh_gfx_glpointer(NULL, (void*)(sizeof(float)*3)));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), Foreground_p->OpenGL.ColorBuffer_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBufferData", 
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), 
        nh_gfx_glsizeiptr(NULL, Foreground_p->Colors.length*sizeof(GLfloat)),
        nh_gfx_glubyte(NULL, Foreground_p->Colors.p, Foreground_p->Colors.length*sizeof(GLfloat)),
        nh_gfx_glenum(NULL, GL_DYNAMIC_DRAW));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glEnableVertexAttribArray", nh_gfx_gluint(NULL, 2));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glVertexAttribPointer",
        nh_gfx_gluint(NULL, 2), nh_gfx_gluint(NULL, 4), nh_gfx_glenum(NULL, GL_FLOAT),
        nh_gfx_glboolean(NULL, GL_FALSE), nh_gfx_glsizei(NULL, sizeof(float)*4), 
        nh_gfx_glpointer(NULL, NULL));

    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindVertexArray", Foreground_p->OpenGL.VertexArray2_p);

    // Indices.
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ELEMENT_ARRAY_BUFFER), Foreground_p->OpenGL.IndicesBuffer2_p);
    Foreground_p->OpenGL.BufferElevatedData_p = nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBufferData",
        nh_gfx_glenum(NULL, GL_ELEMENT_ARRAY_BUFFER),
        nh_gfx_glsizeiptr(NULL, Foreground_p->Indices2.length*sizeof(uint32_t)),
        nh_gfx_glubyte(NULL, Foreground_p->Indices2.p, Foreground_p->Indices2.length*sizeof(uint32_t)),
        nh_gfx_glenum(NULL, GL_STATIC_DRAW));
 
    // Vertices.
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), Foreground_p->OpenGL.VerticesBuffer2_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBufferData", 
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), 
        nh_gfx_glsizeiptr(NULL, Foreground_p->Vertices2.length*sizeof(GLfloat)),
        nh_gfx_glubyte(NULL, Foreground_p->Vertices2.p, Foreground_p->Vertices2.length*sizeof(GLfloat)),
        nh_gfx_glenum(NULL, GL_STATIC_DRAW));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glEnableVertexAttribArray", nh_gfx_gluint(NULL, 0));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glVertexAttribPointer",
        nh_gfx_gluint(NULL, 0), nh_gfx_gluint(NULL, 3), nh_gfx_glenum(NULL, GL_FLOAT),
        nh_gfx_glboolean(NULL, GL_FALSE), nh_gfx_glsizei(NULL, sizeof(float)*3), 
        nh_gfx_glpointer(NULL, NULL));
     nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBindBuffer",
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), Foreground_p->OpenGL.ColorBuffer2_p);
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glBufferData", 
        nh_gfx_glenum(NULL, GL_ARRAY_BUFFER), 
        nh_gfx_glsizeiptr(NULL, Foreground_p->Colors2.length*sizeof(GLfloat)),
        nh_gfx_glubyte(NULL, Foreground_p->Colors2.p, Foreground_p->Colors2.length*sizeof(GLfloat)),
        nh_gfx_glenum(NULL, GL_DYNAMIC_DRAW));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glEnableVertexAttribArray", nh_gfx_gluint(NULL, 1));
    nh_gfx_addOpenGLCommand(CommandBuffer_p, "glVertexAttribPointer",
        nh_gfx_gluint(NULL, 1), nh_gfx_gluint(NULL, 4), nh_gfx_glenum(NULL, GL_FLOAT),
        nh_gfx_glboolean(NULL, GL_FALSE), nh_gfx_glsizei(NULL, sizeof(float)*4), 
        nh_gfx_glpointer(NULL, NULL));

    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_updateOpenGLForeground(
    void *Config_p, void *state_p, void *data_p)
{
    tk_terminal_GraphicsState *State_p = state_p;
    tk_terminal_GraphicsData *Data_p = data_p;

    if (Data_p->Foreground.Action.init) {
        tk_terminal_initOpenGLFontTextures(&Data_p->Foreground.OpenGL, 
            State_p->Viewport_p->OpenGL.CommandBuffer_p);
        tk_terminal_initOpenGLForegroundPrograms(&Data_p->Foreground.OpenGL, 
            State_p->Viewport_p->OpenGL.CommandBuffer_p);
        tk_terminal_initOpenGLForegroundVertices(&Data_p->Foreground.OpenGL, 
            State_p->Viewport_p->OpenGL.CommandBuffer_p);
        Data_p->Foreground.Action.init = false;
    }

    tk_terminal_updateOpenGLForegroundFont(
        Config_p, State_p, &Data_p->Foreground, State_p->Viewport_p->OpenGL.CommandBuffer_p);

    tk_terminal_updateOpenGLForegroundVertices(
        &Data_p->Foreground, State_p->Viewport_p->OpenGL.CommandBuffer_p);

    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_initOpenGLForeground(
    tk_terminal_OpenGLForeground *Foreground_p)
{
    memset(Foreground_p, 0, sizeof(tk_terminal_OpenGLForeground));
 
    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_freeOpenGLForeground(
    tk_terminal_OpenGLForeground *Foreground_p)
{
    nh_gfx_freeOpenGLCommand(Foreground_p->VertexShader_p);
    nh_gfx_freeOpenGLCommand(Foreground_p->VertexShader2_p);
    nh_gfx_freeOpenGLCommand(Foreground_p->FragmentShader_p);
    nh_gfx_freeOpenGLCommand(Foreground_p->FragmentShader2_p);
    nh_gfx_freeOpenGLCommand(Foreground_p->Program_p);
    nh_gfx_freeOpenGLCommand(Foreground_p->Program2_p);
    nh_gfx_freeOpenGLCommand(Foreground_p->GetUniformLocationTexture_p);
 
    nh_gfx_freeOpenGLData(Foreground_p->Texture_p);
    nh_gfx_freeOpenGLData(Foreground_p->BoldTexture_p);
    nh_gfx_freeOpenGLData(Foreground_p->VertexArray_p);
    nh_gfx_freeOpenGLData(Foreground_p->VertexArray2_p);
    nh_gfx_freeOpenGLData(Foreground_p->IndicesBuffer_p);
    nh_gfx_freeOpenGLData(Foreground_p->IndicesBuffer2_p);
    nh_gfx_freeOpenGLData(Foreground_p->VerticesBuffer_p);
    nh_gfx_freeOpenGLData(Foreground_p->VerticesBuffer2_p);
    nh_gfx_freeOpenGLData(Foreground_p->ColorBuffer_p);
    nh_gfx_freeOpenGLData(Foreground_p->ColorBuffer2_p);
 
    return TK_TERMINAL_SUCCESS;
}
