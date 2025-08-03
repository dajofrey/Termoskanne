#ifndef TK_API_TK_TERMINAL_H
#define TK_API_TK_TERMINAL_H

// INCLUDES ========================================================================================

#include "tk-core.h"
#include <stdbool.h>

// ENUMS ===========================================================================================

typedef enum TK_TERMINAL_RESULT
{
    TK_TERMINAL_SUCCESS,
    TK_TERMINAL_ERROR_NULL_POINTER,
    TK_TERMINAL_ERROR_BAD_STATE,
    TK_TERMINAL_ERROR_MEMORY_ALLOCATION,
    TK_TERMINAL_ERROR_TERMINFO_DATA_CANNOT_BE_FOUND,
    TK_TERMINAL_ERROR_UNKNOWN_TERMINAL_TYPE,
    TK_TERMINAL_ERROR_TERMINAL_IS_HARDCOPY,
    TK_TERMINAL_ERROR_UNKNOWN_COMMAND,
    TK_TERMINAL_ERROR_INVALID_ARGUMENT,

} TK_TERMINAL_RESULT;

// TYPEDEFS ========================================================================================

typedef struct tk_terminal_Terminal tk_terminal_Terminal;

// FUNCTIONS =======================================================================================

tk_terminal_Terminal *tk_api_openTerminal(
    char *config_p, tk_core_TTY *TTY_p
);

TK_TERMINAL_RESULT tk_api_setViewport(
    tk_terminal_Terminal *Terminal_p, nh_api_Viewport *Viewport_p
);

#endif // TK_API_TK_TERMINAL_H
