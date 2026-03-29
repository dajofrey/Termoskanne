#ifndef TK_GFX_RENDERER_H
#define TK_GFX_RENDERER_H

#include "Grid.h"
#include "Graphics.h"

#include "../Common/Config.h"
#include "../Common/Includes.h"
#include "../../tk-core/Core/View.h"
#include "../../tk-core/Core/Session.h"

#include "nh-core/Util/Time.h"

typedef struct tk_gfx_Renderer {
    tk_core_Session *Session_p;
    nh_gfx_Text Text;
    char namespace_p[255];
    tk_gfx_Config Config;
    tk_gfx_Grid Grid;
    tk_gfx_Grid ElevatedGrid;
    tk_gfx_Grid BackdropGrid;
    tk_gfx_Graphics Graphics;
    tk_core_View *View_p;
    bool ctrl;
    bool leftMouse;
    nh_core_SystemTime LastClick;
} tk_gfx_Renderer;

#endif 
