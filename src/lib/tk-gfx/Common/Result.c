// LICENSE NOTICE ==================================================================================

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES =========================================================================================

#include "Result.h"

// RESULTS =========================================================================================

const char *TK_API_RESULTS_PP[] = 
{
    "TK_API_SUCCESS",                    
    "TK_API_ERROR_NULL_POINTER",         
    "TK_API_ERROR_BAD_STATE", 
    "TK_API_ERROR_MEMORY_ALLOCATION",
    "TK_API_ERROR_TERMINFO_DATA_CANNOT_BE_FOUND",
    "TK_API_ERROR_UNKNOWN_TERMINAL_TYPE",
    "TK_API_ERROR_TERMINAL_IS_HARDCOPY",
    "TK_API_ERROR_UNKNOWN_COMMAND",
    "TK_API_ERROR_INVALID_ARGUMENT",
};

unsigned int TK_API_RESULTS_PP_COUNT = sizeof(TK_API_RESULTS_PP) / sizeof(TK_API_RESULTS_PP[0]);

