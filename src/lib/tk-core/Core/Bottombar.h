#ifndef TK_CORE_BOTTOMBAR_H
#define TK_CORE_BOTTOMBAR_H

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

    bool tk_core_handlBottombarMove(
        tk_core_Session *Session_p, int cCol, int cRow
    );

    void tk_core_handleBottombarHit( 
        tk_core_Session *Session_p, nh_api_MouseEvent Event, int cCol
    );

    TK_API_RESULT tk_core_drawBottombar(
        tk_core_Session *Session_p
    );

/** @} */

#endif 
