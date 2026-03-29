#ifndef TK_CORE_TITLEBAR_H
#define TK_CORE_TITLEBAR_H

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

#include "Session.h"
#include "../Common/Includes.h"

/** @addtogroup lib_nhtty_functions
 *  @{
 */

    bool tk_core_handleTitlebarMove(
        tk_core_Session *Session_p, int cCol, int cRow
    );

    void tk_core_handleTitlebarHit( 
        tk_core_Session *Session_p, nh_api_MouseEvent Event, int cCol
    );

    void tk_core_checkTitlebar(
        tk_core_Config *Config_p, tk_core_Titlebar *Titlebar_p, bool *refresh_p
    );

    TK_API_RESULT tk_core_drawTitlebar(
        tk_core_Session *Session_p
    );

/** @} */

#endif 
