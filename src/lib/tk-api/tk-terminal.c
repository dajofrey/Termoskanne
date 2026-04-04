// LICENSE NOTICE ==================================================================================

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "tk-terminal.h"
#include "tk-api.h"

#include "nh-core/Loader/Loader.h"

#include <stdio.h>
#include <stdlib.h>

// FUNCTIONS ========================================================================================

static bool added = false;
static const char name_p[] = "tk-terminal";
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


tk_api_Interface *tk_api_createTerminalInterface()
{
    if (!tk_api_add()) {return NULL;}
    typedef tk_api_Interface *(*tk_terminal_createTerminalInterface_f)(); 
    nh_core_Loader *Loader_p = nh_api_getLoader();
    tk_terminal_createTerminalInterface_f createTerminalInterface_f = !Loader_p ? NULL : Loader_p->loadExternalSymbol_f(name_p, "tk_terminal_createTerminalInterface");
printf("%p\n", createTerminalInterface_f);
    return createTerminalInterface_f ? createTerminalInterface_f() : NULL;
}
