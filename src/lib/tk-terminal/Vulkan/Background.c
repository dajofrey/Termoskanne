// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Background.h"

#include "../Common/Macros.h"

#include "nh-core/Util/Array.h"
#include "nh-gfx/Vulkan/Texture.h"
#include "nh-gfx/Common/Macros.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// FUNCTIONS =======================================================================================

static TK_TERMINAL_RESULT tk_terminal_createBackgroundVertices(
    nh_gfx_VulkanDriver *Driver_p, tk_terminal_VulkanBackground *Bg_p, nh_core_Array *Vertices_p, nh_core_Array *Indices_p)
{
    VkBufferCreateInfo BufferCreateInfo =
    {
        .sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size        = Vertices_p->length * sizeof(float),
        .usage       = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    nh_gfx_VulkanBufferInfo BufferInfo =
    {
        .Info_p           = &BufferCreateInfo,
        .data_p           = Vertices_p->p,
        .mapMemory        = true,
        .memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        .createInfo       = false
    };

    NH_CORE_CHECK_2(TK_TERMINAL_ERROR_BAD_STATE, nh_gfx_createVulkanBuffer(
        Driver_p, &BufferInfo, &Bg_p->VertexBuffer
    ))

    BufferCreateInfo.size  = Indices_p->length * sizeof(uint32_t);
    BufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    BufferInfo.data_p = Indices_p->p;
    BufferInfo.Info_p = &BufferCreateInfo;

    NH_CORE_CHECK_2(TK_TERMINAL_ERROR_BAD_STATE, nh_gfx_createVulkanBuffer(
        Driver_p, &BufferInfo, &Bg_p->IndexBuffer
    ))

    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_initVulkanBackground(
    nh_gfx_VulkanGPU *GPU_p, tk_terminal_VulkanBackground *Bg_p)
{
    Bg_p->destroy = false;
    return TK_TERMINAL_SUCCESS;
}

TK_TERMINAL_RESULT tk_terminal_updateVulkanBackground(
    nh_gfx_VulkanGPU *GPU_p, tk_terminal_VulkanBackground *Bg_p, nh_core_Array *Vertices_p, nh_core_Array *Indices_p)
{
    if (Bg_p->destroy) {
        nh_gfx_destroyVulkanBuffer(&GPU_p->Driver, &Bg_p->VertexBuffer);
        nh_gfx_destroyVulkanBuffer(&GPU_p->Driver, &Bg_p->IndexBuffer);
    }

    TK_TERMINAL_CHECK(tk_terminal_createBackgroundVertices(&GPU_p->Driver, Bg_p, Vertices_p, Indices_p))
    Bg_p->destroy = true;

    return TK_TERMINAL_SUCCESS;
}

//TK_TERMINAL_RESULT tk_terminal_destroyVulkanBackground(
//    nh_gfx_VulkanGPU *GPU_p, tk_terminal_VulkanBackground *Bg_p)
//{
//    nh_gfx_destroyVulkanBuffer(&GPU_p->Driver, &Bg_p->VertexBuffer);
//    nh_gfx_destroyVulkanBuffer(&GPU_p->Driver, &Bg_p->IndexBuffer);
//    return NH_SUCCESS;
//}

