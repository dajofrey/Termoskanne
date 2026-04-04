#ifndef TK_API_GFX_H
#define TK_API_GFX_H

// INCLUDES ========================================================================================

#include "tk-core.h"
#include <stdbool.h>

typedef struct tk_api_Renderer tk_api_Renderer;

// FUNCTIONS =======================================================================================

tk_api_Renderer *tk_api_openRenderer(
    char *config_p, tk_api_Session *Session_p
);

TK_API_RESULT tk_api_setViewport(
    tk_api_Renderer *Renderer_p, nh_api_Viewport *Viewport_p
);

#endif
