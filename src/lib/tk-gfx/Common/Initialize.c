// LICENSE NOTICE ==================================================================================

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Initialize.h"
#include "Macros.h"

#if defined(__unix__)
    #include "../Vulkan/Pipeline.h"
    #include "nh-gfx/Vulkan/GPU.h"
    #include "nh-gfx/Vulkan/Vulkan.h"
#endif

#include "nh-core/Config/Config.h"
#include "nh-core/System/Memory.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// DEBUG ===========================================================================================

TK_API_RESULT tk_gfx_initialize()
{
#if defined(__unix__)
    for (int i = 0; i < NH_VULKAN.GPUs.size; ++i) 
    {
        nh_gfx_VulkanGPU *GPU_p = NH_VULKAN.GPUs.pp[0];

        GPU_p->Term.Pipelines_p = (nh_gfx_VulkanPipeline*)nh_core_allocate(sizeof(nh_gfx_VulkanPipeline) * _TK_GFX_VULKAN_PIPELINE_COUNT);
        TK_GFX_CHECK_MEM(GPU_p->Term.Pipelines_p)
 
        TK_GFX_CHECK(tk_gfx_createVulkanPipelines(&GPU_p->Driver, GPU_p->Term.Pipelines_p))
    }
#endif
    return TK_API_SUCCESS;
}
