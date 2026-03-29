#ifndef TK_GFX_LOG_H
#define TK_GFX_LOG_H

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

#include "Includes.h"

#endif

/** @addtogroup lib_nhterminal_functions
 *  @{
 */

    TK_API_RESULT _tk_gfx_logBegin(
        const char *file_p, const char *function_p
    );

    TK_API_RESULT _tk_gfx_logEnd(
        const char *file_p, const char *function_p
    );

    TK_API_RESULT _tk_gfx_logDiagnosticEnd(
        const char *file_p, const char *function_p, TK_API_RESULT result, int line
    );

/** @} */

#endif 
