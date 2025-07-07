#ifndef TK_TERMINAL_VULKAN_RENDER_H
#define TK_TERMINAL_VULKAN_RENDER_H

#include "../Terminal/Graphics.h"
#include "../Common/Includes.h"

/** @addtogroup lib_nh-css_functions 
 *  @{
 */

    TK_TERMINAL_RESULT tk_terminal_renderUsingVulkan(
        tk_terminal_Config *Config_p, tk_terminal_Graphics *Graphics_p, tk_terminal_Grid *Grid_p,
        tk_terminal_Grid *BackdropGrid_p
    );

/** @} */

#endif
