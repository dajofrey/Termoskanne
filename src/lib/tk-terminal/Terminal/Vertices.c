// LICENSE NOTICE ==================================================================================

/**
 * Terminator - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Vertices.h"

#include "../Common/Log.h"
#include "../Common/Macros.h"

#include "nh-gfx/Base/Viewport.h"
#include "nh-gfx/Fonts/HarfBuzz.h"

#include "nh-core/System/Memory.h"
#include "nh-core/Util/Math.h"
#include "nh-core/Util/Array.h"
#include "nh-core/Util/List.h"

#include "nh-encoding/Encodings/UTF8.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// FUNCTIONS =======================================================================================

TK_TERMINAL_RESULT tk_terminal_getBackgroundVertices(
    tk_terminal_GraphicsState *State_p, tk_terminal_Grid *Grid_p, tk_core_Glyph *Glyph_p, int col, 
    int row, float vertices_p[12], int fontSize)
{
    nh_Vertex Vertices_p[4];

    float depth = Glyph_p->mark & TK_CORE_MARK_ELEVATED ? 0.15f : 0.5f;
    int pixel = row * Grid_p->TileSize.height;

    float x = (float)((float)(col * Grid_p->TileSize.width) / (float)Grid_p->Size.width) * 2.0f - 1.0f;
    x -= (float)((float)Grid_p->xOffset/(float)Grid_p->Size.width)*2.0f;

    float y = (float)((float)(pixel) / (float)Grid_p->Size.height) * 2.0f - 1.0f;
    y -= (float)((float)Grid_p->yOffset/(float)Grid_p->Size.height)*2.0f;

    float width  = (((float)Grid_p->TileSize.width)/((float)Grid_p->Size.width))*2;
    float height = (((float)Grid_p->TileSize.height)/((float)Grid_p->Size.height))*2;

    float y1 = y;
    float y2 = y + height;

    if (State_p->Viewport_p->Surface_p->api == NH_GFX_API_OPENGL) {
        y1 = -y1;
        y2 = -y2;
    }

    Vertices_p[0].x = x; 
    Vertices_p[0].y = y1;
    Vertices_p[0].z = depth;

    Vertices_p[1].x = x; 
    Vertices_p[1].y = y2;
    Vertices_p[1].z = depth;

    Vertices_p[2].x = x + width; 
    Vertices_p[2].y = y2;
    Vertices_p[2].z = depth;

    Vertices_p[3].x = x + width; 
    Vertices_p[3].y = y1;
    Vertices_p[3].z = depth;

    nh_verticesToArray(Vertices_p, vertices_p, 4, false, 0);

    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_getBoxVertices(
    tk_terminal_GraphicsState *State_p, tk_terminal_Grid *Grid_p, tk_terminal_Box *Box_p, 
    bool inner, int fontSize)
{
    nh_core_Array Vertices = nh_core_initArray(sizeof(nh_Vertex), 8);

    float depth = 0.2f;
    int pixel = Box_p->UpperLeft.y * Grid_p->TileSize.height;

    float x = (float)((float)(Box_p->UpperLeft.x * Grid_p->TileSize.width) / (float)Grid_p->Size.width) * 2.0f - 1.0f;
    float y = (float)((float)(pixel) / (float)Grid_p->Size.height) * 2.0f - 1.0f;
    float width  = (((float)Grid_p->TileSize.width) / ((float)Grid_p->Size.width))
        * 2 * (Box_p->LowerRight.x - Box_p->UpperLeft.x);
    float height = (((float)Grid_p->TileSize.height) / ((float)Grid_p->Size.height))
        * 2 * (Box_p->LowerRight.y - Box_p->UpperLeft.y);
    float w = (((float)Grid_p->TileSize.width) / ((float)Grid_p->Size.width));
    float h = (((float)Grid_p->TileSize.height) / ((float)Grid_p->Size.height));

    float y1 = y;
    float y2 = y + height;

    if (State_p->Viewport_p->Surface_p->api == NH_GFX_API_OPENGL) {
        y1 = -y1;
        y2 = -y2;
    }

    nh_Vertex *Vertex_p = (nh_Vertex*)nh_core_incrementArray(&Vertices);
    Vertex_p->x = x + w;
    Vertex_p->y = y1 + h;
    Vertex_p->z = depth;

    Vertex_p = (nh_Vertex*)nh_core_incrementArray(&Vertices);
    Vertex_p->x = x + w;
    Vertex_p->y = y2 - h;
    Vertex_p->z = depth;

    Vertex_p = (nh_Vertex*)nh_core_incrementArray(&Vertices);
    Vertex_p->x = x + width - w;
    Vertex_p->y = y2 - h;
    Vertex_p->z = depth;

    Vertex_p = (nh_Vertex*)nh_core_incrementArray(&Vertices);
    Vertex_p->x = x + w;
    Vertex_p->y = y1 + h;
    Vertex_p->z = depth;

    Vertex_p = (nh_Vertex*)nh_core_incrementArray(&Vertices);
    Vertex_p->x = x + width - w;
    Vertex_p->y = y2 - h;
    Vertex_p->z = depth;

    Vertex_p = (nh_Vertex*)nh_core_incrementArray(&Vertices);
    Vertex_p->x = x + width - w;
    Vertex_p->y = y1 + h;
    Vertex_p->z = depth;

    nh_verticesToArray((nh_Vertex*)Vertices.p, inner ? Box_p->innerVertices_p : Box_p->outerVertices_p, Vertices.length, false, 0);
    nh_core_freeArray(&Vertices);

    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_getOutlineVertices(
    tk_terminal_GraphicsState *State_p, tk_terminal_Grid *Grid_p, tk_terminal_Box *Box_p,
    bool inner, int fontSize)
{
    nh_Vertex Vertices_p[6];

    float depth = inner ? 0.4f : 0.45f;
    int pixel = Box_p->UpperLeft.y * Grid_p->TileSize.height;

    float x = (float)((float)(Box_p->UpperLeft.x * Grid_p->TileSize.width) / (float)Grid_p->Size.width) * 2.0f - 1.0f;
    float y = (float)((float)(pixel) / (float)Grid_p->Size.height) * 2.0f - 1.0f;
    float width  = (((float)Grid_p->TileSize.width) / ((float)Grid_p->Size.width)) * 2 * 3;
    float height = (((float)Grid_p->TileSize.height) / ((float)Grid_p->Size.height)) * 2 * 1;
    float w = (((float)Grid_p->TileSize.width) / ((float)Grid_p->Size.width))*2.0f;

    if (inner) {
        x += 2.0f/((float)Grid_p->Size.width);
        width -= (2.0f/((float)Grid_p->Size.width))*2.0f;
        y += 2.0f/((float)Grid_p->Size.height);
        height -= (2.0f/((float)Grid_p->Size.height))*2.0f;
    }

    float y1 = y;
    float y2 = y + height;

    if (State_p->Viewport_p->Surface_p->api == NH_GFX_API_OPENGL) {
        y1 = -y1;
        y2 = -y2;
    }

    Vertices_p[0].x = x + w;
    Vertices_p[0].y = y1;
    Vertices_p[0].z = depth;

    Vertices_p[1].x = x + w;
    Vertices_p[1].y = y2;
    Vertices_p[1].z = depth;

    Vertices_p[2].x = x + width - w;
    Vertices_p[2].y = y2;
    Vertices_p[2].z = depth;

    Vertices_p[3].x = x + w;
    Vertices_p[3].y = y1;
    Vertices_p[3].z = depth;

    Vertices_p[4].x = x + width - w;
    Vertices_p[4].y = y2;
    Vertices_p[4].z = depth;

    Vertices_p[5].x = x + width - w;
    Vertices_p[5].y = y1;
    Vertices_p[5].z = depth;

    nh_verticesToArray(Vertices_p, inner ? Box_p->innerVertices_p : Box_p->outerVertices_p, 6, false, 0);

    return TK_TERMINAL_SUCCESS;
}

static TK_TERMINAL_RESULT tk_terminal_getForegroundVerticesDefault(
    tk_terminal_GraphicsState *State_p, tk_terminal_Grid *Grid_p, tk_core_Glyph *G_p, int col, 
    int row, float depth, float vertices_p[20])
{
    char p[4] = {0};
    nh_encoding_encodeUTF8Single(G_p->codepoint, p);
    nh_gfx_Glyph *Glyph_p = NULL;

    if (G_p->Attributes.bold) {
        Glyph_p = nh_core_getFromHashMap(&State_p->BoldMap, p);
    } else {
        Glyph_p = nh_core_getFromHashMap(&State_p->RegularMap, p);
    }

    if (!Glyph_p) {
        if (G_p->Attributes.bold && nh_gfx_loadGlyphs(State_p->BoldFontInstance_p, &G_p->codepoint, 1)) {
            return TK_TERMINAL_ERROR_BAD_STATE;
        } else if (nh_gfx_loadGlyphs(State_p->RegularFontInstance_p, &G_p->codepoint, 1)) {
            return TK_TERMINAL_ERROR_BAD_STATE;
        }
    
        unsigned int glyphs = 0;
        nh_gfx_HarfBuzzBuffer Buffer;

        if (G_p->Attributes.bold) {
            Buffer = nh_gfx_createHarfBuzzBuffer(State_p->BoldFontInstance_p, &G_p->codepoint, 1);
        } else {
            Buffer = nh_gfx_createHarfBuzzBuffer(State_p->RegularFontInstance_p, &G_p->codepoint, 1);
        }
        nh_gfx_HarfBuzzGlyphInfo *Infos_p = nh_gfx_getHarfBuzzGlyphInfos(Buffer, &glyphs);
    
        if (glyphs != 1) {return TK_TERMINAL_ERROR_BAD_STATE;}

        nh_gfx_Glyph Glyph;
        if (G_p->Attributes.bold) {
            Glyph = nh_gfx_getGlyph(State_p->BoldFontInstance_p, Infos_p[0].id);
        } else {
            Glyph = nh_gfx_getGlyph(State_p->RegularFontInstance_p, Infos_p[0].id);
        }

        Glyph_p = (nh_gfx_Glyph*)nh_core_allocate(sizeof(nh_gfx_Glyph));
        TK_TERMINAL_CHECK_MEM(Glyph_p)
        char *codepoint_p = nh_core_allocate(sizeof(char)*4);
        TK_TERMINAL_CHECK_MEM(codepoint_p)
        memcpy(codepoint_p, p, 4);

        *Glyph_p = Glyph;

        if (G_p->Attributes.bold) {
            nh_core_addToHashMap(&State_p->BoldMap, codepoint_p, Glyph_p);
        } else {
            nh_core_addToHashMap(&State_p->RegularMap, codepoint_p, Glyph_p);
        }

        nh_core_appendToList(&State_p->Glyphs, Glyph_p);
        nh_core_appendToList(&State_p->Codepoints, codepoint_p);
 
        nh_core_free(Infos_p);
        nh_gfx_destroyHarfBuzzBuffer(Buffer);
    }

    int pixel = 0;
    if (G_p->Attributes.bold) {
        pixel = row * Grid_p->TileSize.height + abs(State_p->BoldFontInstance_p->descender);
        pixel += State_p->BoldFontInstance_p->ascender + State_p->BoldFontInstance_p->descender;
    } else {
        pixel = row * Grid_p->TileSize.height + abs(State_p->RegularFontInstance_p->descender);
        pixel += State_p->RegularFontInstance_p->ascender + State_p->RegularFontInstance_p->descender;
    }

    pixel -= Glyph_p->yOffset;

    float x = (float)((float)((col * Grid_p->TileSize.width)+Glyph_p->xOffset) / (float)Grid_p->Size.width) * 2.0f - 1.0f;
    float y = (float)((float)(pixel) / (float)Grid_p->Size.height) * 2.0f - 1.0f;
    float width  = (float)((float)(Glyph_p->width) / (float)Grid_p->Size.width) * 2.0f;
    float height = (float)((float)(Glyph_p->height) / (float)Grid_p->Size.height) * 2.0f;

    float y1 = y;
    float y2 = y + height;

    if (State_p->Viewport_p->Surface_p->api == NH_GFX_API_OPENGL) {
        y1 = -y1;
        y2 = -y2;
    }

    nh_Vertex Vertices_p[4];
 
    Vertices_p[0].x = x; 
    Vertices_p[0].y = y1;
    Vertices_p[0].z = depth;
    Vertices_p[0].u = Glyph_p->u0;
    Vertices_p[0].v = Glyph_p->v0;

    Vertices_p[1].x = x; 
    Vertices_p[1].y = y2;
    Vertices_p[1].z = depth;
    Vertices_p[1].u = Glyph_p->u0;
    Vertices_p[1].v = Glyph_p->v1;

    Vertices_p[2].x = x + width; 
    Vertices_p[2].y = y2;
    Vertices_p[2].z = depth;
    Vertices_p[2].u = Glyph_p->u1;
    Vertices_p[2].v = Glyph_p->v1;

    Vertices_p[3].x = x + width; 
    Vertices_p[3].y = y1;
    Vertices_p[3].z = depth;
    Vertices_p[3].u = Glyph_p->u1;
    Vertices_p[3].v = Glyph_p->v0;

    nh_verticesToArray(Vertices_p, vertices_p, 4, true, 0);

    return TK_TERMINAL_SUCCESS;
}

static TK_TERMINAL_RESULT tk_terminal_getForegroundVerticesForLineGraphics(
    tk_terminal_GraphicsState *State_p, tk_terminal_Grid *Grid_p, NH_API_UTF32 codepoint, int col,
    int row, float depth, float vertices_p[24], int fontSize)
{
    nh_Vertex Vertices_p[8];

    int tmp = row * Grid_p->TileSize.height;

    float x = (float)((float)(col * Grid_p->TileSize.width) / (float)Grid_p->Size.width) * 2.0f - 1.0f;
    x -= (float)((float)Grid_p->xOffset/(float)Grid_p->Size.width)*2.0f;

    float y = (float)((float)(tmp) / (float)Grid_p->Size.height) * 2.0f - 1.0f;
    y -= (float)((float)Grid_p->yOffset/(float)Grid_p->Size.height)*2.0f;

    float width  = (float)((float)(Grid_p->TileSize.width) / (float)Grid_p->Size.width) * 2.0f;
    float height = (float)((float)(Grid_p->TileSize.height) / (float)Grid_p->Size.height) * 2.0f;

    float lineWidthInPixel = (float)Grid_p->borderPixel;
    float w = (lineWidthInPixel / (float)Grid_p->Size.width) * 2.0f;
    float h = (lineWidthInPixel / (float)Grid_p->Size.height) * 2.0f;

    float y1, y2, y3, y4;
    float x1, x2, x3, x4;

    switch (codepoint)
    {
        case 'l' : // |-
            // First rectangle.
            y1 = (y + (height/2)) - h/2;
            y2 = y + height;
            x1 = (x + (width/2)) - w/2;
            x2 = (x + (width/2)) + w/2;
            // Second rectangle.
            y3 = (y + (height/2)) - h/2;
            y4 = (y + (height/2)) + h/2;
            x3 = x2;
            x4 = x + width;
            break;

        case 'k' : // -|
            // First rectangle.
            y1 = (y + (height/2)) - h/2;
            y2 = y + height;
            x1 = (x + (width/2)) - w/2;
            x2 = (x + (width/2)) + w/2;
            // Second rectangle.
            y3 = (y + (height/2)) - h/2;
            y4 = (y + (height/2)) + h/2;
            x3 = x1;
            x4 = x;
            break;

        case 'j' : // -|
            // First rectangle.
            y1 = y;
            y2 = (y + (height/2)) + h/2;
            x1 = (x + (width/2)) - w/2;
            x2 = (x + (width/2)) + w/2;
            // Second rectangle.
            y3 = (y + (height/2)) - h/2;
            y4 = (y + (height/2)) + h/2;
            x3 = x1;
            x4 = x;
            break;

        case 'm' : // |-
            // First rectangle.
            y1 = y;
            y2 = (y + (height/2)) + h/2;
            x1 = (x + (width/2)) - w/2;
            x2 = (x + (width/2)) + w/2;
            // Second rectangle.
            y3 = (y + (height/2)) - h/2;
            y4 = (y + (height/2)) + h/2;
            x3 = x2;
            x4 = x + width;
            break;

        case 'x' : // |
            // First rectangle.
            y1 = y;
            y2 = y + height;
            x1 = (x + (width/2)) - w/2;
            x2 = (x + (width/2)) + w/2;
            // Second rectangle.
            y3 = y1;
            y4 = y2;
            x3 = x1;
            x4 = x2;
            break;

        case 'q' : // -
            // First rectangle.
            y1 = (y + (height/2)) - h/2;
            y2 = (y + (height/2)) + h/2;
            x1 = x;
            x2 = x + width;
            // Second rectangle.
            y3 = y1;
            y4 = y2;
            x3 = x1;
            x4 = x2;
            break;

        case 'n' : // +
            // First rectangle.
            y1 = y;
            y2 = y + height;
            x1 = (x + (width/2)) - w/2;
            x2 = (x + (width/2)) + w/2;
            // Second rectangle.
            y3 = (y + (height/2)) - h/2;
            y4 = (y + (height/2)) + h/2;
            x3 = x;
            x4 = x + width;
            break;

        case 'w' : // T
            // First rectangle.
            y1 = (y + (height/2)) - h/2;
            y2 = (y + (height/2)) + h/2;
            x1 = x;
            x2 = x + width;
            // Second rectangle.
            y3 = y2;
            y4 = y + height;
            x3 = (x + (width/2)) - w/2;
            x4 = (x + (width/2)) + w/2;
            break;

        case 't' : // |-
            // First rectangle.
            y1 = y;
            y2 = y + height;
            x1 = (x + (width/2)) - w/2;
            x2 = (x + (width/2)) + w/2;
            // Second rectangle.
            y3 = (y + (height/2)) - h/2;
            y4 = (y + (height/2)) + h/2;
            x3 = x2;
            x4 = x + width;
            break;

        case 'u' : // -|
            // First rectangle.
            y1 = y;
            y2 = y + height;
            x1 = (x + (width/2)) - w/2;
            x2 = (x + (width/2)) + w/2;
            // Second rectangle.
            y3 = (y + (height/2)) - h/2;
            y4 = (y + (height/2)) + h/2;
            x3 = x1;
            x4 = x;
            break;

        case 'b' : // -| custom with thick vertical line
            // First rectangle.
            y1 = y;
            y2 = y + height;
            x1 = (x + (width/2)) - w/2;
            x2 = x + width;
            // Second rectangle.
            y3 = (y + (height/2)) - h/2;
            y4 = (y + (height/2)) + h/2;
            x3 = x;
            x4 = x + width;
            break;

        case 'c' : // |- custom with thick vertical line
            // First rectangle.
            y1 = y;
            y2 = y + height;
            x1 = x;
            x2 = (x + (width/2)) + w/2;
            // Second rectangle.
            y3 = (y + (height/2)) - h/2;
            y4 = (y + (height/2)) + h/2;
            x3 = x;
            x4 = x + width;
            break;
 
        case 'd' : // | custom with thick vertical line to right
            // First rectangle.
            y1 = y;
            y2 = y + height;
            x1 = (x + (width/2)) - w/2;
            x2 = x + width;
            // Second rectangle.
            y3 = y1;
            y4 = y2;
            x3 = x1;
            x4 = x2;
            break;

        case 'e' : // | custom with thick vertical line to left
            // First rectangle.
            y1 = y;
            y2 = y + height;
            x1 = x;
            x2 = (x + (width/2)) + w/2;
            // Second rectangle.
            y3 = y1;
            y4 = y2;
            x3 = x1;
            x4 = x2;
            break;
 
        case 'v' : // inverse T 
            // First rectangle.
            y1 = (y + (height/2)) - h/2;
            y2 = (y + (height/2)) + h/2;
            x1 = x;
            x2 = x + width;
            // Second rectangle.
            y3 = y;
            y4 = y + (height/2);
            x3 = (x + (width/2)) - w/2;
            x4 = (x + (width/2)) + w/2;
            break;

        case 'z' : // -
            // First rectangle.
            y1 = (y + (height/2)) - h/2;
            y2 = (y + (height/2)) + h/2;
            x1 = x;
            x2 = x + width;
            // Second rectangle.
            y3 = y1;
            y4 = y1 + (height/2);
            x3 = x1;
            x4 = x2;
            break;

        case 'p' : // -
            // First rectangle.
            y1 = (y + (height/2)) - h/2;
            y2 = (y + (height/2)) + h/2;
            x1 = x;
            x2 = x + width;
            // Second rectangle.
            y3 = y1;
            y4 = y1 + (height/4);
            x3 = x1;
            x4 = x2;
            break;

        case 'o' : // -
            // First rectangle.
            y1 = (y) - h/2;
            y2 = (y) + h/2;
            x1 = x;
            x2 = x + width;
            // Second rectangle.
            y3 = y1;
            y4 = y1 + (height/2);
            x3 = x1;
            x4 = x2;
            break;

        case 'f' : // -
            // First rectangle.
            y1 = (y) - h/2;
            y2 = (y) + h/2;
            x1 = x;
            x2 = x + width;
            // Second rectangle.
            y3 = y1;
            y4 = y1 + (height/4);
            x3 = x1;
            x4 = x2;
            break;

    }

    if (State_p->Viewport_p->Surface_p->api == NH_GFX_API_OPENGL) {
        y1 = -y1;
        y2 = -y2;
        y3 = -y3;
        y4 = -y4;
    }

    Vertices_p[0].x = x1;
    Vertices_p[0].y = y1;
    Vertices_p[0].z = depth;

    Vertices_p[1].x = x1;
    Vertices_p[1].y = y2;
    Vertices_p[1].z = depth;

    Vertices_p[2].x = x2;
    Vertices_p[2].y = y2;
    Vertices_p[2].z = depth;

    Vertices_p[3].x = x2;
    Vertices_p[3].y = y1;
    Vertices_p[3].z = depth;

    Vertices_p[4].x = x3;
    Vertices_p[4].y = y3;
    Vertices_p[4].z = depth;

    Vertices_p[5].x = x3; 
    Vertices_p[5].y = y4;
    Vertices_p[5].z = depth;

    Vertices_p[6].x = x4; 
    Vertices_p[6].y = y4;
    Vertices_p[6].z = depth;

    Vertices_p[7].x = x4; 
    Vertices_p[7].y = y3;
    Vertices_p[7].z = depth;

    nh_verticesToArray(Vertices_p, vertices_p, 8, false, 0);

    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_getForegroundVertices(
    tk_terminal_GraphicsState *State_p, tk_terminal_Grid *Grid_p, tk_core_Glyph *Glyph_p, int col,
    int row, float *vertices_p, int fontSize)
{
    float depth = Glyph_p->mark & TK_CORE_MARK_ELEVATED ? 0.1f : 0.2f;

    if (Glyph_p->mark & TK_CORE_MARK_LINE_GRAPHICS) {
        TK_TERMINAL_CHECK(tk_terminal_getForegroundVerticesForLineGraphics(
            State_p, Grid_p, Glyph_p->codepoint, col, row, depth, vertices_p, fontSize
        ))
    } else {
        TK_TERMINAL_CHECK(tk_terminal_getForegroundVerticesDefault(
            State_p, Grid_p, Glyph_p, col, row, depth, vertices_p
        ))
    }

    return TK_TERMINAL_SUCCESS;
}

unsigned int tk_terminal_getSidebarOffset(
    tk_terminal_Grid *Grid_p)
{
    float width = (float)((float)(Grid_p->TileSize.width) / (float)Grid_p->Size.width) * 2.0f;
    float lineWidthInPixel = (float)Grid_p->borderPixel;
    float w = (lineWidthInPixel / (float)Grid_p->Size.width) * 2.0f;
 
    float x = -1.0f;
    x -= (float)((float)Grid_p->xOffset/(float)Grid_p->Size.width)*2.0f;

    float x1 = x;
    float x2 = (x + (width/2)) - w/2;

    float result = ((x2 - x1) * Grid_p->Size.width)/2.0f;
    return (unsigned int) result;
}
