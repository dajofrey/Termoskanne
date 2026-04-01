#ifndef TK_CORE_COMMON_CONFIG_H
#define TK_CORE_COMMON_CONFIG_H

// INCLUDES ========================================================================================

#include "Includes.h"
#include <stddef.h>

// STRUCTS =========================================================================================

typedef struct tk_terminal_Config {
    unsigned int maxScroll;
} tk_terminal_Config;

// FUNCTIONS =======================================================================================

tk_terminal_Config tk_terminal_updateConfig(
    void *Session_p
);

#endif // TK_CORE_COMMON_CONFIG_H
