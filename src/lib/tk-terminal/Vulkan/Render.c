// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Render.h"
#include "Foreground.h"
#include "Background.h"
#include "Dim.h"
#include "Pipeline.h"

#include "../Common/Macros.h"
#include "../Common/Config.h"

#include "nh-gfx/Base/Viewport.h"
#include "nh-gfx/Vulkan/GPU.h"

#include <stdlib.h>
#include <string.h>

// FUNCTIONS =======================================================================================

static void tk_terminal_recordVulkanDim(
    tk_terminal_VulkanDim *Dim_p, nh_gfx_VulkanGPU *GPU_p, VkCommandBuffer *CommandBuffer_p, unsigned int vertices)
{
    GPU_p->Driver.Functions.vkCmdBindPipeline(
        *CommandBuffer_p, VK_PIPELINE_BIND_POINT_GRAPHICS, 
        GPU_p->Term.Pipelines_p[TK_TERMINAL_VULKAN_PIPELINE_BACKGROUND].Pipeline
    );

    VkDeviceSize size_p[1] = {0};

    GPU_p->Driver.Functions.vkCmdBindVertexBuffers(
        *CommandBuffer_p, 0, 1, &Dim_p->VertexBuffer.Buffer, size_p
    );

    GPU_p->Driver.Functions.vkCmdDraw(
        *CommandBuffer_p, vertices, 1, 0, 0
    );
}

static void tk_terminal_recordVulkanClearAlpha(
    nh_gfx_VulkanGPU *GPU_p, VkCommandBuffer *CommandBuffer_p)
{
    GPU_p->Driver.Functions.vkCmdBindPipeline(
        *CommandBuffer_p, VK_PIPELINE_BIND_POINT_GRAPHICS, 
        GPU_p->Term.Pipelines_p[TK_TERMINAL_VULKAN_PIPELINE_CLEAR_ALPHA].Pipeline
    );

    GPU_p->Driver.Functions.vkCmdDraw(
        *CommandBuffer_p, 3, 1, 0, 0
    );
}

static void tk_terminal_recordVulkanBackground(
    tk_terminal_VulkanBackground *Background_p, nh_gfx_VulkanGPU *GPU_p, VkCommandBuffer *CommandBuffer_p, int indices)
{
    GPU_p->Driver.Functions.vkCmdBindPipeline(
        *CommandBuffer_p, VK_PIPELINE_BIND_POINT_GRAPHICS, 
        GPU_p->Term.Pipelines_p[TK_TERMINAL_VULKAN_PIPELINE_BACKGROUND].Pipeline
    );

    VkDeviceSize size_p[1] = {0};

    GPU_p->Driver.Functions.vkCmdBindVertexBuffers(
        *CommandBuffer_p, 0, 1, &Background_p->VertexBuffer.Buffer, size_p
    );

    GPU_p->Driver.Functions.vkCmdBindIndexBuffer(
        *CommandBuffer_p, Background_p->IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32
    );

    GPU_p->Driver.Functions.vkCmdDrawIndexed(
        *CommandBuffer_p, indices, 1, 0, 0, 0
    );
}

static void tk_terminal_recordVulkanForeground(
    tk_terminal_VulkanForeground *Text_p, nh_gfx_VulkanGPU *GPU_p, VkCommandBuffer *CommandBuffer_p, int indices)
{
    GPU_p->Driver.Functions.vkCmdBindPipeline(
        *CommandBuffer_p, VK_PIPELINE_BIND_POINT_GRAPHICS, 
        GPU_p->Term.Pipelines_p[TK_TERMINAL_VULKAN_PIPELINE_SDF].Pipeline
    );
    VkDeviceSize size_p[1] = {0};

    GPU_p->Driver.Functions.vkCmdBindVertexBuffers(
        *CommandBuffer_p, 0, 1, &Text_p->VertexBuffer.Buffer, size_p
    );
    
    GPU_p->Driver.Functions.vkCmdBindDescriptorSets(
        *CommandBuffer_p, VK_PIPELINE_BIND_POINT_GRAPHICS,
        GPU_p->Term.Pipelines_p[TK_TERMINAL_VULKAN_PIPELINE_SDF].PipelineLayout,
        0, 1, &Text_p->DescriptorSet.DescriptorSet, 0, VK_NULL_HANDLE
    );

    GPU_p->Driver.Functions.vkCmdBindIndexBuffer(
        *CommandBuffer_p, Text_p->IndexBuffer.Buffer, 0, VK_INDEX_TYPE_UINT32
    );

    GPU_p->Driver.Functions.vkCmdDrawIndexed(
        *CommandBuffer_p, indices, 1, 0, 0, 0
    );
}

TK_TERMINAL_RESULT tk_terminal_renderUsingVulkan(
    tk_terminal_Config *Config_p, tk_terminal_Graphics *Graphics_p, tk_terminal_Grid *Grid_p,
    tk_terminal_Grid *BackdropGrid_p)
{
    nh_gfx_FontInstance *FontInstance_p = nh_gfx_claimFontInstance(
        Graphics_p->State.Fonts.pp[Graphics_p->State.font], Config_p->fontSize
    );

    TK_TERMINAL_CHECK(tk_terminal_updateVulkanBackground(
        Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p,
        &Graphics_p->BackdropData.Background.Vulkan,
        &Graphics_p->BackdropData.Background.Vertices,
        &Graphics_p->BackdropData.Background.Indices
    ))
    TK_TERMINAL_CHECK(tk_terminal_updateVulkanBackground(
        Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p,
        &Graphics_p->MainData.Background.Vulkan,
        &Graphics_p->MainData.Background.Vertices,
        &Graphics_p->MainData.Background.Indices
    ))
    TK_TERMINAL_CHECK(tk_terminal_updateVulkanForeground(
        Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p,
        FontInstance_p,
        &Graphics_p->MainData.Foreground.Vulkan,
        &Graphics_p->MainData.Foreground.Vertices,
        &Graphics_p->MainData.Foreground.Indices
    ))
    TK_TERMINAL_CHECK(tk_terminal_updateVulkanDim(
        Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p,
        &Graphics_p->Dim.Vulkan,
        &Graphics_p->Dim.Vertices
    ))

    nh_gfx_beginRecording(Graphics_p->State.Viewport_p);

    for (int i = 0; i < Graphics_p->State.Viewport_p->Vulkan.images; ++i) {

        VkViewport viewport = {
            .x = 0.0f,
            .y = 0.0f,
            .width = (float)BackdropGrid_p->Size.width,
            .height = (float)BackdropGrid_p->Size.height,
            .minDepth = 0.0f,
            .maxDepth = 1.0f
        };
        
        Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p->Driver.Functions.vkCmdSetViewport(*Graphics_p->State.Viewport_p->Vulkan.CommandBuffers_pp[i], 0, 1, &viewport);

        tk_terminal_recordVulkanBackground(
            &Graphics_p->BackdropData.Background.Vulkan,
            Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p,
            Graphics_p->State.Viewport_p->Vulkan.CommandBuffers_pp[i],
            Graphics_p->BackdropData.Background.Indices.length
        );

        viewport.x = Grid_p->borderPixel;
        viewport.y = Grid_p->borderPixel;
        viewport.width = (float)Grid_p->Size.width;
        viewport.height = (float)Grid_p->Size.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        
        Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p->Driver.Functions.vkCmdSetViewport(
            *Graphics_p->State.Viewport_p->Vulkan.CommandBuffers_pp[i], 0, 1, &viewport);

        tk_terminal_recordVulkanDim(
            &Graphics_p->Dim.Vulkan,
            Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p,
            Graphics_p->State.Viewport_p->Vulkan.CommandBuffers_pp[i],
            Graphics_p->Dim.Vertices.length/7
        );

        tk_terminal_recordVulkanBackground(
            &Graphics_p->MainData.Background.Vulkan,
            Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p,
            Graphics_p->State.Viewport_p->Vulkan.CommandBuffers_pp[i],
            Graphics_p->MainData.Background.Indices.length
        );

        tk_terminal_recordVulkanForeground(
            &Graphics_p->MainData.Foreground.Vulkan,
            Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p,
            Graphics_p->State.Viewport_p->Vulkan.CommandBuffers_pp[i],
            Graphics_p->MainData.Foreground.Indices.length
        );

        tk_terminal_recordVulkanClearAlpha(
            Graphics_p->State.Viewport_p->Surface_p->Vulkan.GPU_p,
            Graphics_p->State.Viewport_p->Vulkan.CommandBuffers_pp[i]
        );
    }

    nh_gfx_endRecording(Graphics_p->State.Viewport_p, false);

    return TK_TERMINAL_SUCCESS;
}
