#ifndef TK_TERMINAL_VULKAN_BACKGROUND_H
#define TK_TERMINAL_VULKAN_BACKGROUND_H

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

#include "../Common/Includes.h"

#include "nh-gfx/Vulkan/GPU.h"
#include "nh-gfx/Vulkan/Helper.h"
#include "nh-gfx/Fonts/FontManager.h"

#include <stdint.h>

typedef struct tk_terminal_VulkanBackground {
    bool destroy;
    nh_gfx_VulkanBuffer VertexBuffer;
    nh_gfx_VulkanBuffer IndexBuffer;
    nh_gfx_VulkanBuffer VertShaderBuffer;
    nh_gfx_VulkanBuffer FragShaderBuffer;
    nh_gfx_VulkanDescriptorSet DescriptorSet;
} tk_terminal_VulkanBackground;

TK_TERMINAL_RESULT tk_terminal_initVulkanBackground(
    nh_gfx_VulkanGPU *GPU_p, tk_terminal_VulkanBackground *Bg_p
);

TK_TERMINAL_RESULT tk_terminal_updateVulkanBackground(
    nh_gfx_VulkanGPU *GPU_p, tk_terminal_VulkanBackground *Bg_p, nh_core_Array *Vertices_p, nh_core_Array *Indices_p
);

#endif 
