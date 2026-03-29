// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Dim.h"

#include "../Common/Macros.h"

#include "nh-core/Util/Array.h"
#include "nh-gfx/Vulkan/Texture.h"
#include "nh-gfx/Common/Macros.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// FUNCTIONS =======================================================================================

static TK_API_RESULT tk_gfx_createVulkanDimVertices(
    nh_gfx_VulkanDriver *Driver_p, tk_gfx_VulkanDim *Bg_p, nh_core_Array *Vertices_p)
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

    NH_CORE_CHECK_2(TK_API_ERROR_BAD_STATE, nh_gfx_createVulkanBuffer(
        Driver_p, &BufferInfo, &Bg_p->VertexBuffer
    ))

    return TK_API_SUCCESS;
}

TK_API_RESULT tk_gfx_initVulkanDim(
    nh_gfx_VulkanGPU *GPU_p, tk_gfx_VulkanDim *Bg_p)
{
    Bg_p->destroy = false;
    return TK_API_SUCCESS;
}

TK_API_RESULT tk_gfx_updateVulkanDim(
    nh_gfx_VulkanGPU *GPU_p, tk_gfx_VulkanDim *Bg_p, nh_core_Array *Vertices_p)
{
    if (Bg_p->destroy) {
        nh_gfx_destroyVulkanBuffer(&GPU_p->Driver, &Bg_p->VertexBuffer);
    }

    TK_GFX_CHECK(tk_gfx_createVulkanDimVertices(&GPU_p->Driver, Bg_p, Vertices_p))
    Bg_p->destroy = true;

    return TK_API_SUCCESS;
}

//TK_API_RESULT tk_gfx_destroyVulkanBackground(
//    nh_gfx_VulkanGPU *GPU_p, tk_gfx_VulkanBackground *Bg_p)
//{
//    nh_gfx_destroyVulkanBuffer(&GPU_p->Driver, &Bg_p->VertexBuffer);
//    nh_gfx_destroyVulkanBuffer(&GPU_p->Driver, &Bg_p->IndexBuffer);
//    return NH_SUCCESS;
//}
