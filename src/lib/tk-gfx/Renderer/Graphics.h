#ifndef TK_GFX_TERMINAL_GRAPHICS_H
#define TK_GFX_TERMINAL_GRAPHICS_H

// INCLUDES ========================================================================================

#include "Grid.h"

#if defined(__unix__)
    #include "../Vulkan/Foreground.h"
    #include "../Vulkan/Background.h"
    #include "../Vulkan/Dim.h"
#endif

#include "../OpenGL/Foreground.h"
#include "../OpenGL/Background.h"
#include "../OpenGL/Boxes.h"
#include "../OpenGL/Dim.h"

#include "nh-core/Util/HashMap.h"
#include "nh-gfx/Base/Viewport.h"

#include "../../tk-core/Common/Config.h"

// STRUCTS =========================================================================================

typedef struct tk_gfx_AttributeRange {
    tk_core_Glyph Glyph;
    int indices;
} tk_gfx_AttributeRange;

typedef struct tk_gfx_GraphicsAction {
    bool init;
} tk_gfx_GraphicsAction;

typedef struct tk_gfx_Dim {
    tk_gfx_GraphicsAction Action;
    tk_gfx_OpenGLDim OpenGL;
#if defined(__unix__)
    tk_gfx_VulkanDim Vulkan;
#endif
    nh_core_Array Vertices;
} tk_gfx_Dim;

/**
 * Text that is drawn on top of the background. Can be either normal or elevated,
 * which basically is the same but with different depth values.
 */
typedef struct tk_gfx_GraphicsForeground {
    tk_gfx_GraphicsAction Action;
#if defined(__unix__)
    tk_gfx_VulkanForeground Vulkan;
#endif
    tk_gfx_OpenGLForeground OpenGL;
    nh_core_Array Vertices;
    nh_core_Array Indices;
    nh_core_Array Vertices2;
    nh_core_Array Indices2;
    nh_core_Array Ranges;
    nh_core_Array Ranges2;
    nh_core_Array Colors;
    nh_core_Array Colors2;
} tk_gfx_GraphicsForeground;

/**
 * The background behind normal text, boxes and elevated text.
 */
typedef struct tk_gfx_GraphicsBackground {
    tk_gfx_GraphicsAction Action;
    tk_gfx_OpenGLBackground OpenGL;
#if defined(__unix__)
    tk_gfx_VulkanBackground Vulkan;
#endif
    nh_core_Array Vertices;
    nh_core_Array Indices;
    nh_core_Array Ranges;
    nh_core_Array Colors;
} tk_gfx_GraphicsBackground;

typedef struct tk_gfx_Boxes {
    tk_gfx_GraphicsAction Action;
    tk_gfx_OpenGLBoxes OpenGL;
    nh_core_Array Vertices;
    nh_core_Array Colors;
    nh_core_Array Data;
} tk_gfx_Boxes;

typedef struct tk_gfx_GraphicsGradient {
    tk_core_Color Color;
    float ratio;
    int index;
    double interval;
    nh_core_SystemTime LastChange;
} tk_gfx_GraphicsGradient;

typedef struct tk_gfx_GraphicsData {
    tk_gfx_GraphicsForeground Foreground;
    tk_gfx_GraphicsBackground Background;
    bool update;
} tk_gfx_GraphicsData;

/**
 * Blinking state.
 */
typedef struct tk_gfx_GraphicsBlink {
    bool on;              /**<If true, cursor is in visible period.*/
    nh_core_SystemTime LastBlink; /**<System time of last blink change.*/
} tk_gfx_GraphicsBlink;

typedef struct tk_gfx_GraphicsState {
    nh_gfx_Viewport *Viewport_p;
    tk_gfx_GraphicsBlink Blink;
    tk_gfx_GraphicsGradient AccentGradient;
    tk_gfx_GraphicsGradient BackgroundGradient;
    nh_core_HashMap RegularMap;
    nh_core_HashMap BoldMap;
    nh_gfx_FontInstance *RegularFontInstance_p;
    nh_gfx_FontInstance *BoldFontInstance_p;
    nh_core_List Fonts;
    int font;
    nh_core_List Glyphs;
    nh_core_List Codepoints;
} tk_gfx_GraphicsState;

typedef struct tk_gfx_Graphics {
    tk_gfx_GraphicsState State;
    tk_gfx_GraphicsData MainData;
    tk_gfx_GraphicsData ElevatedData;
    tk_gfx_GraphicsData BackdropData;
    tk_gfx_Dim Dim;
    tk_gfx_Boxes Boxes;
} tk_gfx_Graphics;

// FUNCTIONS =======================================================================================

TK_API_RESULT tk_gfx_initGraphics(
    tk_gfx_Config *Config_p, tk_gfx_Graphics *Graphics_p
);

TK_API_RESULT tk_gfx_freeGraphics(
    tk_gfx_Graphics *Graphics_p
);

TK_API_RESULT tk_gfx_handleViewportChange(
    tk_gfx_Config *Config_p, tk_gfx_Graphics *Graphics_p, nh_gfx_Viewport *Viewport_p
);

TK_API_RESULT tk_gfx_updateGraphics( 
    tk_gfx_Config *Config_p, tk_gfx_Graphics *Graphics_p, tk_gfx_Grid *Grid_p,
    tk_gfx_Grid *BackdropGrid_p, tk_gfx_Grid *ElevatedGrid_p, bool titlebarOn
); 

TK_API_RESULT tk_gfx_renderGraphics(
    tk_gfx_Config *Config_p, tk_gfx_Graphics *Graphics_p, tk_gfx_Grid *Grid_p,
    tk_gfx_Grid *ElevatedGrid_p, tk_gfx_Grid *BackdropGrid_p, tk_core_Config *CoreConfig_p
);

bool tk_gfx_updateBlinkOrGradient(
    tk_gfx_Config *Config_p, tk_gfx_GraphicsState *State_p
);

#endif 
