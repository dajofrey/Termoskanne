// LICENSE NOTICE ==================================================================================

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "tk-core.h"
#include "tk-api.h"

#include "nh-core/Loader/Loader.h"

#include <stdio.h>
#include <stdlib.h>

// FUNCTIONS ========================================================================================

static bool added = false;
static const char name_p[] = "tk-core";
static const char *dependencies_pp[16] = {
    "nh-encoding",
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

tk_api_Session *tk_api_openSession(
    char *config_p, tk_core_Interface *Interface_p)
{
    if (!tk_api_add()) {return NULL;}
    typedef tk_api_Session *(*tk_core_openSession_f)( char *config_p, tk_core_Interface *Interface_p); 
    nh_core_Loader *Loader_p = nh_api_getLoader();
    tk_core_openSession_f openSession_f = !Loader_p ? NULL : Loader_p->loadExternalSymbol_f(name_p, "tk_core_openSession");
    return openSession_f ? openSession_f(config_p, Interface_p) : NULL;
}

TK_API_RESULT tk_api_closeSession(
    tk_api_Session *Session_p)
{
    if (!tk_api_add()) {return TK_API_ERROR_BAD_STATE;}
    typedef TK_API_RESULT (*tk_core_closeSession_f)(tk_api_Session *Session_p);
    nh_core_Loader *Loader_p = nh_api_getLoader();
    tk_core_closeSession_f closeSession_f = !Loader_p ? NULL : Loader_p->loadExternalSymbol_f(name_p, "tk_core_closeSession");
    return closeSession_f ? closeSession_f(Session_p) : TK_API_ERROR_BAD_STATE;
}

TK_API_RESULT tk_api_claimStandardIO(
    tk_api_Session *Session_p)
{
    if (!tk_api_add()) {return TK_API_ERROR_BAD_STATE;}
    typedef TK_API_RESULT (*tk_core_cmd_claimStandardIO_f)(tk_api_Session *Session_p);
    nh_core_Loader *Loader_p = nh_api_getLoader();
    tk_core_cmd_claimStandardIO_f claimStandardIO_f = !Loader_p ? NULL : Loader_p->loadExternalSymbol_f(name_p, "tk_core_cmd_claimStandardIO");
    return claimStandardIO_f ? claimStandardIO_f(Session_p) : TK_API_ERROR_BAD_STATE;
}

TK_API_RESULT tk_api_unclaimStandardIO(
    tk_api_Session *Session_p)
{
    if (!tk_api_add()) {return TK_API_ERROR_BAD_STATE;}
    typedef TK_API_RESULT (*tk_core_cmd_unclaimStandardIO_f)(tk_api_Session *Session_p); 
    nh_core_Loader *Loader_p = nh_api_getLoader();
    tk_core_cmd_unclaimStandardIO_f unclaimStandardIO_f = !Loader_p ? NULL : Loader_p->loadExternalSymbol_f(name_p, "tk_core_cmd_unclaimStandardIO");
    return unclaimStandardIO_f ? unclaimStandardIO_f(Session_p) : TK_API_ERROR_BAD_STATE;
}

TK_API_RESULT tk_api_sendEvent(
    tk_api_Session *Session_p, nh_api_WSIEvent Event)
{
    if (!tk_api_add()) {return TK_API_ERROR_BAD_STATE;}
    typedef TK_API_RESULT (*tk_core_cmd_sendEvent_f)(tk_api_Session *Session_p, nh_api_WSIEvent Event);
    nh_core_Loader *Loader_p = nh_api_getLoader();
    tk_core_cmd_sendEvent_f sendEvent_f = !Loader_p ? NULL : Loader_p->loadExternalSymbol_f(name_p, "tk_core_cmd_sendEvent");
    return sendEvent_f ? sendEvent_f(Session_p, Event) : TK_API_ERROR_BAD_STATE;
}
