// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Config.h"
#include "Macros.h"

#include "../Core/Session.h"

#include "nh-core/Config/Config.h"
#include "nh-core/System/Memory.h"
#include "nh-core/System/Thread.h"

#include <string.h>
#include <stdlib.h>

// FUNCTIONS =======================================================================================

static TK_API_RESULT tk_terminal_getSetting(
    tk_terminal_Config *Config_p, char *namespace_p, int index, char *setting_p)
{
    nh_core_List *Values_p = nh_core_getGlobalConfigSetting(namespace_p, setting_p);
    TK_CHECK_NULL(Values_p)

    switch (index) {
        case 0 :
            if (Values_p->size != 1) {return TK_API_ERROR_BAD_STATE;}
            Config_p->maxScroll = atoi(Values_p->pp[0]);
            break;
    }

    return TK_API_SUCCESS;
}

static tk_terminal_Config tk_terminal_getStaticConfig()
{
    tk_terminal_Config Config;
    memset(&Config, 0, sizeof(tk_terminal_Config));

    static const char *options_pp[] = {
        "tk-terminal.maxScroll",
    };

    int options = sizeof(options_pp)/sizeof(options_pp[0]);

    tk_core_Session *Session_p = nh_core_getWorkloadArg();
    TK_CHECK_NULL_2(Config, Session_p)

    for (int i = 0; i < options; ++i) {
        TK_CHECK_2(Config, tk_terminal_getSetting(&Config, Session_p->namespace_p, i, options_pp[i]))
    }

    return Config;
}

tk_terminal_Config tk_terminal_updateConfig( 
    void *Session_p) 
{ 
    ((tk_core_Session*)Session_p)->Config = tk_terminal_getStaticConfig(); 
    return ((tk_core_Session*)Session_p)->Config; 
} 
