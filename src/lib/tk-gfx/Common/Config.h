#ifndef TK_GFX_CONFIG_H
#define TK_GFX_CONFIG_H

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

#include "Includes.h"
#include <stddef.h>

typedef struct tk_gfx_Config {
    int fontSize;
    float blinkFrequency;
    float animationFreq;
    tk_core_Color Foreground;
    tk_core_Color Backgrounds_p[8];
    tk_core_Color Accents_p[8];
    tk_core_Color Highlight;
    int accents;
    int backgrounds;
    int style;
    int border;
    int highContrast;
} tk_gfx_Config;

tk_gfx_Config tk_gfx_updateConfig(
    void *Renderer_p
);

tk_gfx_Config tk_gfx_updateConfigIfMarked(
    void *Renderer_p
);

tk_gfx_Config tk_gfx_getConfig(
    void *Renderer_p
);

#endif
