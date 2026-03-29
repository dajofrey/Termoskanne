// LICENSE NOTICE ==================================================================================

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "tk-gfx.h"
#include "tk-api.h"

#include "nh-core/Loader/Loader.h"

// FUNCTIONS =======================================================================================

static bool added = false;
static const char name_p[] = "tk-gfx";
static const char *dependencies_pp[16] = {
    "nh-gfx",
    "tk-core",
};

static bool tk_api_add() {
    nh_core_Loader *Loader_p = nh_api_getLoader();
    if (Loader_p == NULL) {
        return false;
    }
    if (!added) {
        Loader_p->addModule_f(name_p, TK_API_PATH_P, dependencies_pp, 1);
        added = true;
    }
    return added;
}

tk_api_Renderer *tk_api_openRenderer(
    char *config_p, tk_api_Session *Session_p)
{
    typedef tk_api_Renderer *(*tk_gfx_openRenderer_f)(char *config_p, tk_api_Session *Session_p);
    if (!tk_api_add()) {return NULL;}
    nh_core_Loader *Loader_p = nh_api_getLoader();
    tk_gfx_openRenderer_f openRenderer_f = !Loader_p || !Session_p ? NULL : Loader_p->loadExternalSymbol_f(name_p, "tk_gfx_openRenderer");
    return openRenderer_f ? openRenderer_f(config_p, Session_p) : NULL;
}

TK_API_RESULT tk_api_setViewport(
    tk_api_Renderer *Renderer_p, nh_api_Viewport *Viewport_p)
{
    typedef TK_API_RESULT (*tk_gfx_cmd_setViewport_f)(tk_api_Renderer *Renderer_p, nh_api_Viewport *Viewport_p); 
    if (!tk_api_add()) {return TK_API_ERROR_BAD_STATE;}
    nh_core_Loader *Loader_p = nh_api_getLoader();
    tk_gfx_cmd_setViewport_f setViewport_f = !Loader_p || !Renderer_p || !Viewport_p ? NULL : Loader_p->loadExternalSymbol_f(name_p, "tk_gfx_cmd_setViewport");
    return setViewport_f ? setViewport_f(Renderer_p, Viewport_p) : TK_API_ERROR_NULL_POINTER;
}
