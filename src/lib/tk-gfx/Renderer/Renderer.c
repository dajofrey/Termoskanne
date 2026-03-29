// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Renderer.h"
#include "Vertices.h"

#include "../Common/Macros.h"

#include "nh-core/System/Thread.h"
#include "nh-core/System/Memory.h"
#include "nh-core/System/Process.h"
#include "nh-core/Util/RingBuffer.h"
#include "nh-core/Util/List.h"
#include "nh-core/Config/Config.h"
#include "nh-encoding/Encodings/UTF8.h"
#include "nh-encoding/Encodings/UTF32.h"
#include "nh-gfx/Base/Viewport.h"
#include "nh-gfx/Fonts/FontManager.h"
#include "nh-gfx/Fonts/Text.h"

#include "nh-wsi/Window/Window.h"
#include "nh-wsi/Window/WindowSettings.h"

#include "../../tk-core/Core/Session.h"

#include <string.h>
#include <stdio.h>

// INIT/FREE =======================================================================================
// The next functions comprise the in/exit points of nhterminal.

typedef struct tk_gfx_Args {
    tk_core_Session *Session_p;
    char *namespace_p;
} tk_gfx_Args;

static void *tk_gfx_initRenderer(
    nh_core_Workload *Workload_p)
{
    static char *name_p = "Terminal Emulator";
    static char *path_p = "tk-gfx/Renderer/Renderer.c";

    Workload_p->name_p = name_p;
    Workload_p->path_p = path_p;
    Workload_p->module = -1;

    tk_gfx_Renderer *Renderer_p = (tk_gfx_Renderer*)nh_core_allocate(sizeof(tk_gfx_Renderer));
    TK_GFX_CHECK_MEM_2(NULL, Renderer_p)

    memset(Renderer_p, 0, sizeof(tk_gfx_Renderer));

    tk_gfx_updateConfig(Renderer_p);

    Renderer_p->ctrl = false;
    Renderer_p->leftMouse = false;
    Renderer_p->Session_p = ((tk_gfx_Args*)Workload_p->args_p)->Session_p;
    Renderer_p->View_p = tk_core_createView(Renderer_p->Session_p, Renderer_p, false);

    TK_GFX_CHECK_MEM_2(NULL, Renderer_p->View_p)

    if (((tk_gfx_Args*)Workload_p->args_p)->namespace_p) {
        strcpy(Renderer_p->namespace_p, ((tk_gfx_Args*)Workload_p->args_p)->namespace_p);
    } else {
        sprintf(Renderer_p->namespace_p, "%p", Renderer_p);
    }

    TK_GFX_CHECK_2(NULL, tk_gfx_initGraphics(&Renderer_p->Config, &Renderer_p->Graphics))
 
    TK_GFX_CHECK_2(NULL, tk_gfx_initGrid(&Renderer_p->Grid))
    TK_GFX_CHECK_2(NULL, tk_gfx_initGrid(&Renderer_p->ElevatedGrid))

    return Renderer_p;
}

static void tk_gfx_freeRenderer(
    void *terminal_p)
{
    tk_gfx_Renderer *Renderer_p = terminal_p;

    tk_gfx_freeGraphics(&Renderer_p->Graphics);
    tk_gfx_freeGrid(&Renderer_p->Grid);
    tk_gfx_freeGrid(&Renderer_p->ElevatedGrid);
    tk_gfx_freeGrid(&Renderer_p->BackdropGrid);

    nh_gfx_freeText(&Renderer_p->Text);

    nh_core_free(Renderer_p);
}

// RUN LOOP ========================================================================================
// The next functions comprise the top-level of the nhterminal run loop.

static TK_API_RESULT tk_gfx_updateBoxes( 
    tk_gfx_Renderer *Renderer_p, nh_core_Array *NewBoxes_p, int fontSize)
{ 
    if (Renderer_p->Graphics.Boxes.Data.length > 0) { 
        nh_core_freeArray(&Renderer_p->Graphics.Boxes.Data); 
    } 

    for (int i = 0; i < NewBoxes_p->length; ++i) 
    { 
        tk_gfx_Box *Box_p = (tk_gfx_Box*)nh_core_incrementArray(&Renderer_p->Graphics.Boxes.Data);
        TK_GFX_CHECK_MEM(Box_p) 
        *Box_p = ((tk_gfx_Box*)NewBoxes_p->p)[i]; 

        if (Box_p->UpperLeft.x == Box_p->LowerRight.x && Box_p->UpperLeft.y == Box_p->UpperLeft.y) {
            TK_GFX_CHECK(tk_gfx_getOutlineVertices(&Renderer_p->Graphics.State, &Renderer_p->Grid, Box_p, true, fontSize))
            TK_GFX_CHECK(tk_gfx_getOutlineVertices(&Renderer_p->Graphics.State, &Renderer_p->Grid, Box_p, false, fontSize))
        } else { 
            TK_GFX_CHECK(tk_gfx_getBoxVertices(&Renderer_p->Graphics.State, &Renderer_p->Grid, Box_p, true, fontSize))
            TK_GFX_CHECK(tk_gfx_getBoxVertices(&Renderer_p->Graphics.State, &Renderer_p->Grid, Box_p, false, fontSize))
        } 
    } 

    return TK_API_SUCCESS; 
} 

static TK_API_RESULT tk_gfx_updateSize(
    tk_gfx_Renderer *Renderer_p)
{
    // maybe refactor this updateSurface to Netzhaut..?
    nh_gfx_updateSurface(Renderer_p->Graphics.State.Viewport_p->Surface_p);

    nh_gfx_claimFontInstance(
        Renderer_p->Graphics.State.Fonts.pp[0], Renderer_p->Config.fontSize
    );
    nh_gfx_claimFontInstance(
        Renderer_p->Graphics.State.Fonts.pp[1], Renderer_p->Config.fontSize
    );

    nh_gfx_freeText(&Renderer_p->Text);

    NH_API_UTF32 c = 'e';
    NH_API_RESULT failure = 1;
    for (int i = 0; i < Renderer_p->Graphics.State.Fonts.size; ++i) {
        failure = nh_gfx_createTextFromFont(&Renderer_p->Text, &c, 1, Renderer_p->Config.fontSize,
            Renderer_p->Graphics.State.Fonts.pp[i]);
        if (!failure) {
            Renderer_p->Graphics.State.font = i;
            break;
        }
    }

    if (failure) {return TK_API_ERROR_BAD_STATE;}
    
    TK_GFX_CHECK(tk_gfx_updateGrid(&Renderer_p->Config, &Renderer_p->Session_p->Config, &Renderer_p->Grid, &Renderer_p->Graphics.State, &Renderer_p->Text))
    TK_GFX_CHECK(tk_gfx_updateGrid(&Renderer_p->Config, &Renderer_p->Session_p->Config, &Renderer_p->ElevatedGrid, &Renderer_p->Graphics.State, &Renderer_p->Text))
    TK_GFX_CHECK(tk_gfx_updateBackdropGrid(&Renderer_p->Config, &Renderer_p->Session_p->Config, &Renderer_p->BackdropGrid, &Renderer_p->Graphics.State, &Renderer_p->Text))

    // Update view size, subtract gap tiles.
    Renderer_p->View_p->cols = Renderer_p->Grid.cols-1;
    Renderer_p->View_p->rows = Renderer_p->Grid.rows-1;
    Renderer_p->View_p->Size = Renderer_p->Grid.Size;
    Renderer_p->View_p->TileSize = Renderer_p->Grid.TileSize;

    Renderer_p->Graphics.BackdropData.update = true;
    Renderer_p->Graphics.ElevatedData.update = true;

    TK_GFX_CHECK(tk_gfx_updateGraphics(
        &Renderer_p->Config, &Renderer_p->Graphics, &Renderer_p->Grid, &Renderer_p->BackdropGrid, 
        &Renderer_p->ElevatedGrid, Renderer_p->Session_p->Config.Titlebar.on))

    return TK_API_SUCCESS;
}

static TK_API_RESULT tk_gfx_updateSizeIfRequired(
    tk_gfx_Renderer *Renderer_p, bool *update_p)
{
    if (!Renderer_p->Session_p->Config.Titlebar.on && Renderer_p->Session_p->Config.Topbar.on) {
        if (Renderer_p->Graphics.State.Viewport_p->Settings.Size.width - (Renderer_p->Grid.borderPixel*2) == Renderer_p->Grid.Size.width
        &&  Renderer_p->Graphics.State.Viewport_p->Settings.Size.height - (Renderer_p->Grid.borderPixel*1) == Renderer_p->Grid.Size.height) {
            return TK_API_SUCCESS;
        }
    }
    else if (Renderer_p->Graphics.State.Viewport_p->Settings.Size.width - (Renderer_p->Grid.borderPixel*2) == Renderer_p->Grid.Size.width
         &&  Renderer_p->Graphics.State.Viewport_p->Settings.Size.height - (Renderer_p->Grid.borderPixel*2) == Renderer_p->Grid.Size.height) {
        return TK_API_SUCCESS;
    }

    TK_GFX_CHECK(tk_gfx_updateSize(Renderer_p))

    *update_p = true;

    return TK_API_SUCCESS;
}

static bool tk_gfx_updateConfigIfRequired(
    tk_gfx_Renderer *Renderer_p)
{
    tk_gfx_Config OldConfig = Renderer_p->Config;
    tk_gfx_updateConfigIfMarked(Renderer_p);

    if (OldConfig.fontSize != Renderer_p->Config.fontSize) {

        nh_gfx_unclaimFontInstance(nh_gfx_claimFontInstance(
            Renderer_p->Graphics.State.Fonts.pp[0], Renderer_p->Config.fontSize));
        nh_gfx_unclaimFontInstance(nh_gfx_claimFontInstance(
            Renderer_p->Graphics.State.Fonts.pp[1], Renderer_p->Config.fontSize));

        tk_gfx_updateSize(Renderer_p);

        return true;
    }

    if (OldConfig.style != Renderer_p->Config.style) {

        TK_GFX_CHECK(tk_gfx_updateBackdropGrid(
            &Renderer_p->Config, &Renderer_p->Session_p->Config, &Renderer_p->BackdropGrid,
            &Renderer_p->Graphics.State, &Renderer_p->Text))

        Renderer_p->Graphics.BackdropData.update = true;
        Renderer_p->Graphics.ElevatedData.update = true;
    
        TK_GFX_CHECK(tk_gfx_updateGraphics(
            &Renderer_p->Config, &Renderer_p->Graphics, &Renderer_p->Grid, &Renderer_p->BackdropGrid, 
            &Renderer_p->ElevatedGrid, Renderer_p->Session_p->Config.Titlebar.on))
 
        return true;
    }

    if (OldConfig.Backgrounds_p[0].r != Renderer_p->Config.Backgrounds_p[0].r
     || OldConfig.Backgrounds_p[0].g != Renderer_p->Config.Backgrounds_p[0].g
     || OldConfig.Backgrounds_p[0].b != Renderer_p->Config.Backgrounds_p[0].b) {

        TK_GFX_CHECK(tk_gfx_updateBackdropGrid(
            &Renderer_p->Config, &Renderer_p->Session_p->Config, &Renderer_p->BackdropGrid,
            &Renderer_p->Graphics.State, &Renderer_p->Text))

        Renderer_p->Graphics.BackdropData.update = true;
        Renderer_p->Graphics.ElevatedData.update = true;
 
        TK_GFX_CHECK(tk_gfx_updateGraphics(
            &Renderer_p->Config, &Renderer_p->Graphics, &Renderer_p->Grid, &Renderer_p->BackdropGrid, 
            &Renderer_p->ElevatedGrid, Renderer_p->Session_p->Config.Titlebar.on))
 
        return true;
    }

    if (OldConfig.border != Renderer_p->Config.border) {

        tk_gfx_updateSize(Renderer_p);

        TK_GFX_CHECK(tk_gfx_updateBackdropGrid(
            &Renderer_p->Config, &Renderer_p->Session_p->Config, &Renderer_p->BackdropGrid,
            &Renderer_p->Graphics.State, &Renderer_p->Text))

        Renderer_p->Graphics.BackdropData.update = true;
        Renderer_p->Graphics.ElevatedData.update = true;
    
        TK_GFX_CHECK(tk_gfx_updateGraphics(
            &Renderer_p->Config, &Renderer_p->Graphics, &Renderer_p->Grid, &Renderer_p->BackdropGrid, 
            &Renderer_p->ElevatedGrid, Renderer_p->Session_p->Config.Titlebar.on))
 
        return true;
    }


    return false;
}

static TK_API_RESULT tk_gfx_handleEvent(
    tk_gfx_Renderer *Renderer_p, nh_api_WSIEvent *Event_p)
{
    if (Event_p->type == NH_API_WSI_EVENT_MOUSE) {
        if (Event_p->Mouse.type == NH_API_MOUSE_SCROLL && Renderer_p->ctrl) {
            int newFontSize = Renderer_p->Config.fontSize + (Event_p->Mouse.trigger == NH_API_TRIGGER_UP ? 1 : -1);

            if (newFontSize < 10 || newFontSize > 60) {
                // Out of bounds.
                return TK_API_SUCCESS;
            }

            nh_core_overwriteGlobalConfigSettingInt(
                Renderer_p->namespace_p, "tk-gfx.font.size", newFontSize);

            TK_GFX_CHECK(tk_gfx_updateConfigIfRequired(Renderer_p))
        }
        if (Event_p->Mouse.type == NH_API_MOUSE_LEFT && Event_p->Mouse.trigger == NH_API_TRIGGER_PRESS) {
            Renderer_p->leftMouse = true;
            nh_core_SystemTime Now = nh_core_getSystemTime();
            if (nh_core_getSystemTimeDiffInSeconds(Renderer_p->LastClick, Now) <= 0.3f) {
                nh_wsi_toggleWindowSize_f toggleWindowSize_f = nh_core_loadExistingSymbol(NH_MODULE_WSI, 0, "nh_wsi_toggleWindowSize");
                if (toggleWindowSize_f && Event_p->Mouse.Position.y < Renderer_p->Grid.TileSize.height) {
                    toggleWindowSize_f(Renderer_p->Graphics.State.Viewport_p->Surface_p->Window_p);
                } 
            }
            Renderer_p->LastClick = Now;
        } 
        if (Event_p->Mouse.type == NH_API_MOUSE_LEFT && Event_p->Mouse.trigger == NH_API_TRIGGER_RELEASE) {
            Renderer_p->leftMouse = false;
        } 
        if (Event_p->Mouse.type == NH_API_MOUSE_MOVE && Renderer_p->leftMouse) {
            if (Event_p->Mouse.Position.y < Renderer_p->Grid.TileSize.height) {
                nh_api_moveWindow((nh_api_Window*)Renderer_p->Graphics.State.Viewport_p->Surface_p->Window_p);
                if (Renderer_p->Graphics.State.Viewport_p->Surface_p->Window_p->type == NH_WSI_TYPE_X11) {
                    Renderer_p->leftMouse = false;
                }
            } 
        }
    }

    if (Event_p->type == NH_API_WSI_EVENT_KEYBOARD) {
        if (Event_p->Keyboard.special == NH_API_KEY_CONTROL_L || Event_p->Keyboard.special == NH_API_KEY_CONTROL_R) {
            if (Event_p->Keyboard.trigger == NH_API_TRIGGER_PRESS) {
                Renderer_p->ctrl = true;
            }
            if (Event_p->Keyboard.trigger == NH_API_TRIGGER_RELEASE) {
                Renderer_p->ctrl = false;
            }
        }
    }

    if (Event_p->type == NH_API_WSI_EVENT_WINDOW) {
        Renderer_p->leftMouse = false;
    }

    return TK_API_SUCCESS;
}

static TK_API_RESULT tk_gfx_handleInputIfRequired(
    tk_gfx_Renderer *Renderer_p, bool *update_p)
{
    nh_core_Array *Array_p = NULL;
    nh_api_WSIEvent *Event_p = NULL;

    for (int row = 0; row < Renderer_p->Grid.rows; ++row) {
        memset(Renderer_p->Grid.updates_pp[row], false, Renderer_p->Grid.cols*sizeof(bool));
    }
    for (int row = 0; row < Renderer_p->ElevatedGrid.rows; ++row) {
        memset(Renderer_p->ElevatedGrid.updates_pp[row], false, Renderer_p->ElevatedGrid.cols*sizeof(bool));
    }

    do {
        Array_p = (nh_core_Array*)nh_core_incrementRingBufferMarker(
            &Renderer_p->View_p->Forward.Tiles, &Renderer_p->View_p->Forward.Tiles.Marker);
        if (!Array_p) {break;}
        for (int i = 0; i < Array_p->length; ++i) {
            tk_gfx_TileUpdate *Update_p = ((tk_gfx_TileUpdate*)Array_p->p)+i;
            if (Update_p->row >= Renderer_p->Grid.rows || Update_p->col >= Renderer_p->Grid.cols) {
                // Ignore invalid update.
                continue;
            }
            if (Update_p->cursor) {
                TK_GFX_CHECK(tk_gfx_updateTile(
                    &Renderer_p->Grid, &Renderer_p->Graphics.State, Update_p, update_p, Renderer_p->Config.fontSize))
            } else if (Update_p->Glyph.mark & TK_CORE_MARK_ELEVATED) {
                Renderer_p->ElevatedGrid.Updates_pp[Update_p->row][Update_p->col] = *Update_p;
                Renderer_p->ElevatedGrid.updates_pp[Update_p->row][Update_p->col] = true;
            } else {
                Renderer_p->Grid.Updates_pp[Update_p->row][Update_p->col] = *Update_p;
                Renderer_p->Grid.updates_pp[Update_p->row][Update_p->col] = true;
            }
        }
    } while (Array_p);

    // Update tiles.
    for (int row = 0; row < Renderer_p->Grid.rows; ++row) {
        for (int col = 0; col < Renderer_p->Grid.cols; ++col) {
            if (Renderer_p->Grid.updates_pp[row][col] == false) {continue;}
            TK_GFX_CHECK(tk_gfx_updateTile(
                &Renderer_p->Grid, &Renderer_p->Graphics.State, &Renderer_p->Grid.Updates_pp[row][col],
                update_p, Renderer_p->Config.fontSize))
            // update backdrop grid tile if necessarry
            if (row == 0 && Renderer_p->Session_p->Config.Topbar.on && !Renderer_p->Session_p->Config.Titlebar.on) {
                tk_gfx_TileUpdate Update = Renderer_p->Grid.Updates_pp[row][col];
                Update.row = 2;
                Update.col += 1;
                bool updateBackdrop = false;
                if (Update.Glyph.codepoint == 'x') {
                    TK_GFX_CHECK(tk_gfx_updateTile(
                        &Renderer_p->BackdropGrid, &Renderer_p->Graphics.State, &Update, &updateBackdrop, Renderer_p->Config.fontSize))
                } else {
                    Update.Glyph.codepoint = 0;
                    Update.Glyph.mark = TK_CORE_MARK_ACCENT | TK_CORE_MARK_LINE_GRAPHICS;
                    Update.Glyph.Attributes.reverse = true;
                    TK_GFX_CHECK(tk_gfx_updateTile(
                        &Renderer_p->BackdropGrid, &Renderer_p->Graphics.State, &Update, &updateBackdrop, Renderer_p->Config.fontSize))
                }
                if (updateBackdrop) {
                    Renderer_p->Graphics.BackdropData.update = true;
                    *update_p = true;
                }
            }
        }
    }

    for (int row = 0; row < Renderer_p->ElevatedGrid.rows; ++row) {
        for (int col = 0; col < Renderer_p->ElevatedGrid.cols; ++col) {
            if (Renderer_p->ElevatedGrid.updates_pp[row][col] == false) {continue;}
            bool update = false;
            TK_GFX_CHECK(tk_gfx_updateTile(
                &Renderer_p->ElevatedGrid, &Renderer_p->Graphics.State, &Renderer_p->ElevatedGrid.Updates_pp[row][col],
                &update, Renderer_p->Config.fontSize))
            if (update) {
                *update_p = true;
                Renderer_p->Graphics.ElevatedData.update = true;
            }
        }
    }

    do {
        Array_p = (nh_core_Array*)nh_core_incrementRingBufferMarker(
            &Renderer_p->View_p->Forward.Boxes, &Renderer_p->View_p->Forward.Boxes.Marker);
        if (!Array_p) {break;}
        TK_GFX_CHECK(tk_gfx_updateBoxes(Renderer_p, Array_p, Renderer_p->Config.fontSize))
    } while (Array_p);

    do {
        Event_p = (nh_api_WSIEvent*)nh_core_incrementRingBufferMarker(
            &Renderer_p->View_p->Forward.Events, &Renderer_p->View_p->Forward.Events.Marker);
        if (!Event_p) {break;}
        TK_GFX_CHECK(tk_gfx_handleEvent(Renderer_p, Event_p))
    } while (Event_p);

    return TK_API_SUCCESS;
}

static NH_SIGNAL tk_gfx_runRenderer(
    void *args_p) 
{
    TK_GFX_CHECK_NULL_2(NH_SIGNAL_ERROR, args_p)
    tk_gfx_Renderer *Renderer_p = args_p;

    if (!Renderer_p->Graphics.State.Viewport_p) {return NH_SIGNAL_IDLE;}

    bool update = tk_gfx_updateConfigIfRequired(Renderer_p);

    TK_GFX_CHECK_2(NH_SIGNAL_ERROR, tk_gfx_updateSizeIfRequired(Renderer_p, &update))
    TK_GFX_CHECK_2(NH_SIGNAL_ERROR, tk_gfx_handleInputIfRequired(Renderer_p, &update))
    if (tk_gfx_updateBlinkOrGradient(&Renderer_p->Config, &Renderer_p->Graphics.State)) {
        update = true;
    }

    Renderer_p->Graphics.State.Viewport_p->Settings.BorderColor.r = Renderer_p->Graphics.State.AccentGradient.Color.r;
    Renderer_p->Graphics.State.Viewport_p->Settings.BorderColor.g = Renderer_p->Graphics.State.AccentGradient.Color.g;
    Renderer_p->Graphics.State.Viewport_p->Settings.BorderColor.b = Renderer_p->Graphics.State.AccentGradient.Color.b;
 
    if (update) {
        TK_GFX_CHECK_2(NH_SIGNAL_ERROR, tk_gfx_updateGraphics(
            &Renderer_p->Config, &Renderer_p->Graphics, &Renderer_p->Grid, &Renderer_p->BackdropGrid, 
            &Renderer_p->ElevatedGrid, Renderer_p->Session_p->Config.Titlebar.on))
        TK_GFX_CHECK_2(NH_SIGNAL_ERROR, tk_gfx_renderGraphics(&Renderer_p->Config, &Renderer_p->Graphics, 
            &Renderer_p->Grid, &Renderer_p->ElevatedGrid, &Renderer_p->BackdropGrid, &Renderer_p->Session_p->Config))
        return NH_SIGNAL_OK;
    }

    return update == true ? NH_SIGNAL_OK : NH_SIGNAL_IDLE;
}

// COMMANDS ========================================================================================
// The next functions are executed by tk_gfx_cmd_* functions.

typedef enum TK_GFX_COMMAND_E {
    TK_GFX_COMMAND_SET_VIEWPORT,
} TK_GFX_COMMAND_E;

/**
 * Here, most commands that came in through the API are handled.
 */
static NH_SIGNAL tk_gfx_runRendererCommand(
    void *terminal_p, nh_core_WorkloadCommand *Command_p)
{
    tk_gfx_Renderer *Renderer_p = terminal_p;

    switch (Command_p->type)
    {
        case TK_GFX_COMMAND_SET_VIEWPORT :
            if (nh_gfx_claimViewport(Command_p->p, NH_GFX_VIEWPORT_OWNER_TERMINAL, Renderer_p) != NH_API_SUCCESS) {
                return NH_SIGNAL_ERROR;
            }
            tk_gfx_handleViewportChange(&Renderer_p->Config, &Renderer_p->Graphics, Command_p->p);
            break;
    }

    return NH_SIGNAL_OK;
}

// API =============================================================================================
// The next functions are called by lib/netzhaut/nhterminal.h functions.

tk_gfx_Renderer *tk_gfx_openRenderer(
    char *namespace_p, tk_core_Session *Session_p)
{
    tk_gfx_Args Args;
    Args.Session_p = Session_p;
    Args.namespace_p = namespace_p;

    tk_gfx_Renderer *Renderer_p = nh_core_activateWorkload(
        tk_gfx_initRenderer, tk_gfx_runRenderer, tk_gfx_freeRenderer,
        tk_gfx_runRendererCommand, &Args, true);

    Session_p->Renderer_p = Renderer_p;

    return Renderer_p;
}

TK_API_RESULT tk_gfx_cmd_setViewport(
    tk_gfx_Renderer *Renderer_p, nh_gfx_Viewport *Viewport_p)
{
    nh_core_executeWorkloadCommand(Renderer_p, TK_GFX_COMMAND_SET_VIEWPORT, Viewport_p, 0);

    return TK_API_SUCCESS;
}
