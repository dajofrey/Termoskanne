// LICENSE NOTICE ==================================================================================

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Config.h"
#include "Macros.h"

#include "../Renderer/Renderer.h"

#include "nh-core/Config/Config.h"
#include "nh-core/System/Memory.h"
#include "nh-core/System/Thread.h"

#include <string.h>
#include <stdlib.h>

// FUNCTIONS =======================================================================================

static TK_API_RESULT tk_gfx_getSetting(
    tk_gfx_Config *Config_p, char *namespace_p, int setting, char *setting_p)
{
    nh_core_List *Setting_p = nh_core_getGlobalConfigSetting(namespace_p, setting_p);
    TK_GFX_CHECK_NULL(Setting_p)

    switch (setting) {
        case 0 :
            if (Setting_p->size != 1) {return TK_API_ERROR_BAD_STATE;}
            Config_p->fontSize = atoi(Setting_p->pp[0]);
            break;
        case 1 :
            if (Setting_p->size != 1) {return TK_API_ERROR_BAD_STATE;}
            Config_p->blinkFrequency = atof(Setting_p->pp[0]);
            break;
        case 2 :
            if (Setting_p->size != 4) {return TK_API_ERROR_BAD_STATE;}
            Config_p->Foreground.r = ((float)atoi(Setting_p->pp[0]))/255.0f;
            Config_p->Foreground.g = ((float)atoi(Setting_p->pp[1]))/255.0f;
            Config_p->Foreground.b = ((float)atoi(Setting_p->pp[2]))/255.0f;
            Config_p->Foreground.a = ((float)atoi(Setting_p->pp[3]))/255.0f;
            break;
        case 3 :
            Config_p->backgrounds = 0;
            if (Setting_p->size < 4 || (Setting_p->size % 4) != 0) {return TK_API_ERROR_BAD_STATE;}
            for (int i = 0, j = 0; j < Setting_p->size; i++, j += 4) {
                Config_p->Backgrounds_p[i].r = ((float)atof(Setting_p->pp[j+0]))/255.0f;
                Config_p->Backgrounds_p[i].g = ((float)atof(Setting_p->pp[j+1]))/255.0f;
                Config_p->Backgrounds_p[i].b = ((float)atof(Setting_p->pp[j+2]))/255.0f;
                Config_p->Backgrounds_p[i].a = ((float)atof(Setting_p->pp[j+3]))/255.0f;
                Config_p->backgrounds++;
            }
            break;
        case 4 :
            Config_p->accents = 0;
            if (Setting_p->size < 4 || (Setting_p->size % 4) != 0) {return TK_API_ERROR_BAD_STATE;}
            for (int i = 0, j = 0; j < Setting_p->size; i++, j += 4) {
                Config_p->Accents_p[i].r = ((float)atof(Setting_p->pp[j+0]))/255.0f;
                Config_p->Accents_p[i].g = ((float)atof(Setting_p->pp[j+1]))/255.0f;
                Config_p->Accents_p[i].b = ((float)atof(Setting_p->pp[j+2]))/255.0f;
                Config_p->Accents_p[i].a = ((float)atof(Setting_p->pp[j+3]))/255.0f;
                Config_p->accents++;
            }
            break;
        case 5 :
            if (Setting_p->size != 1) {return TK_API_ERROR_BAD_STATE;}
            Config_p->style = atoi(Setting_p->pp[0]);
            break;
        case 6 :
            if (Setting_p->size != 1) {return TK_API_ERROR_BAD_STATE;}
            Config_p->animationFreq = atof(Setting_p->pp[0]);
            break;
	case 7 :
	    if (Setting_p->size != 1) {return TK_API_ERROR_BAD_STATE;}
	    Config_p->border = atoi(Setting_p->pp[0]);
	    break;
	case 8 :
	    if (Setting_p->size != 1) {return TK_API_ERROR_BAD_STATE;}
	    Config_p->highContrast = atoi(Setting_p->pp[0]);
	    break;
        case 9 :
            if (Setting_p->size != 4) {return TK_API_ERROR_BAD_STATE;}
            Config_p->Highlight.r = ((float)atoi(Setting_p->pp[0]))/255.0f;
            Config_p->Highlight.g = ((float)atoi(Setting_p->pp[1]))/255.0f;
            Config_p->Highlight.b = ((float)atoi(Setting_p->pp[2]))/255.0f;
            Config_p->Highlight.a = ((float)atoi(Setting_p->pp[3]))/255.0f;
            break;
    }

    return TK_API_SUCCESS;
}

static tk_gfx_Config tk_gfx_getStaticConfig()
{
    tk_gfx_Config Config;
    memset(&Config, 0, sizeof(tk_gfx_Config));

    static const char *options_pp[] = {
        "tk-gfx.font.size",
        "tk-gfx.timer.blink",
        "tk-gfx.color.foreground",
        "tk-gfx.color.background",
        "tk-gfx.color.accent",
        "tk-gfx.style",
        "tk-gfx.animate",
	"tk-gfx.border",
	"tk-gfx.high_contrast",
        "tk-gfx.color.highlight",
    };

    int options = sizeof(options_pp)/sizeof(options_pp[0]);
 
    tk_gfx_Renderer *Renderer_p = nh_core_getWorkloadArg();
    TK_GFX_CHECK_NULL_2(Config, Renderer_p)

    for (int i = 0; i < options; ++i) {
        TK_GFX_CHECK_2(Config, tk_gfx_getSetting(&Config, Renderer_p->namespace_p, i, options_pp[i]))
    }

    return Config;
}

tk_gfx_Config tk_gfx_updateConfig(
    void *Renderer_p) 
{
    ((tk_gfx_Renderer*)Renderer_p)->Config = tk_gfx_getStaticConfig(); 
    return ((tk_gfx_Renderer*)Renderer_p)->Config;
}

tk_gfx_Config tk_gfx_updateConfigIfMarked(
    void *Renderer_p) 
{
    bool mark = nh_core_popGlobalConfigMark(((tk_gfx_Renderer*)Renderer_p)->namespace_p);
    if (mark) {
        return tk_gfx_updateConfig(Renderer_p);
    }
    return ((tk_gfx_Renderer*)Renderer_p)->Config;
}
