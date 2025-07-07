#ifndef TK_TERMINAL_VULKAN_DIM_H
#define TK_TERMINAL_VULKAN_DIM_H

#include "../Common/Includes.h"

#include "nh-gfx/Vulkan/GPU.h"
#include "nh-gfx/Vulkan/Helper.h"
#include "nh-gfx/Fonts/FontManager.h"

#include <stdint.h>

typedef struct tk_terminal_VulkanDim {
    bool destroy;
    nh_gfx_VulkanBuffer VertexBuffer;
    nh_gfx_VulkanBuffer VertShaderBuffer;
    nh_gfx_VulkanBuffer FragShaderBuffer;
    nh_gfx_VulkanDescriptorSet DescriptorSet;
} tk_terminal_VulkanDim;

TK_TERMINAL_RESULT tk_terminal_initVulkanDim(
    nh_gfx_VulkanGPU *GPU_p, tk_terminal_VulkanDim *Bg_p
);

TK_TERMINAL_RESULT tk_terminal_updateVulkanDim(
    nh_gfx_VulkanGPU *GPU_p, tk_terminal_VulkanDim *Bg_p, nh_core_Array *Vertices_p
);

#endif 
