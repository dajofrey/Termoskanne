#ifndef TK_CORE_TERMINAL_TERMINAL_H
#define TK_CORE_TERMINAL_TERMINAL_H

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

#include "../Common/Includes.h"

#include <stddef.h>
#include <unistd.h>

tk_api_Interface *tk_core_createTerminalInterface(
);

#endif 
