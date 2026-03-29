#ifndef TK_GFX_VULKAN_RENDER_H
#define TK_GFX_VULKAN_RENDER_H

#include "../Terminal/Graphics.h"
#include "../Common/Includes.h"

/** @addtogroup lib_nh-css_functions 
 *  @{
 */

    TK_API_RESULT tk_gfx_renderUsingVulkan(
        tk_gfx_Config *Config_p, tk_gfx_Graphics *Graphics_p, tk_gfx_Grid *Grid_p,
        tk_gfx_Grid *BackdropGrid_p
    );

/** @} */

#endif
