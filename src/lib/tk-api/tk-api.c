// LICENSE NOTICE ==================================================================================

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "tk-api.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#if defined(__APPLE__)
    #include <dlfcn.h>
#elif defined(__unix__)
    #include <elf.h>
    #include <link.h>
#endif

// FUNCTIONS =======================================================================================

char TK_API_PATH_P[255] = {0};

void tk_api_initialize() 
{
#if defined(__unix__)
    const ElfW(Dyn) *dyn = _DYNAMIC;
    const ElfW(Dyn) *rpath = NULL;
    const ElfW(Dyn) *runpath = NULL;
    const char *strtab = NULL;
    char path_p[1024] = {};

    for (; dyn->d_tag != DT_NULL; ++dyn) {
        if (dyn->d_tag == DT_RPATH) {
            rpath = dyn;
        } else if (dyn->d_tag == DT_RUNPATH) {
            runpath = dyn;
        } else if (dyn->d_tag == DT_STRTAB) {
            strtab = (const char *)dyn->d_un.d_val;
        }
    }

    assert(strtab != NULL);

    if (rpath != NULL) {
        sprintf(TK_API_PATH_P, strtab + rpath->d_un.d_val);
    } else if (runpath != NULL) {
        sprintf(TK_API_PATH_P, strtab + runpath->d_un.d_val);
    }
#elif defined(__APPLE__)
    Dl_info info;
    if (dladdr((void*)&tk_api_initialize, &info) && info.dli_fname) {
        snprintf(TK_API_PATH_P, sizeof(TK_API_PATH_P), "%s", info.dli_fname);
    } else {
        snprintf(TK_API_PATH_P, sizeof(TK_API_PATH_P), "<unknown>");
    }
#endif
}
