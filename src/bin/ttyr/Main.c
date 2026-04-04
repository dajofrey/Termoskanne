// LICENSE NOTICE ==================================================================================

/**
 * Monoco - Text-based Tooling
 * Copyright (C) 2022  Dajo Frey
 * Published under MIT.
 */

// INCLUDES ========================================================================================

#include "tk-api/tk-api.h"
#include "nh-api/nh-api.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// TYPES ===========================================================================================

static nh_api_PixelPosition Position = {0};
static nh_api_Viewport *Viewport_p = NULL;
static tk_api_Session *Session_p = NULL;

// HELPER ==========================================================================================

static void handleInput(
    nh_api_Window *Window_p, nh_api_WSIEvent Event)
{
    switch (Event.type)
    {
        case NH_API_WSI_EVENT_MOUSE :
        case NH_API_WSI_EVENT_KEYBOARD :
            tk_api_sendEvent(Session_p, Event);
            break;
        case NH_API_WSI_EVENT_WINDOW :
            switch (Event.Window.type) 
            {
                case NH_API_WINDOW_CONFIGURE :
                    if (Viewport_p) {
                        nh_api_configureViewport(Viewport_p, Position, Event.Window.PixelSize); 
                    }
                    break;
                case NH_API_WINDOW_FOCUS_OUT :
                case NH_API_WINDOW_FOCUS_IN :
                    tk_api_sendEvent(Session_p, Event);
                    break;
            }
            break;
    }
}

// MAIN ============================================================================================
 
int main(int argc, char **argv_pp) 
{
    switch (argc)
    {
        case 2:
            if (nh_api_initialize(NULL, argv_pp[1], strlen(argv_pp[1])) != NH_API_SUCCESS) {
                puts("Netzhaut API initialization failed. Exiting.\n");
                return 1;
            }
            break;
        case 1:
            if (nh_api_initialize(NULL, NULL, 0) != NH_API_SUCCESS) {
                puts("Netzhaut API initialization failed. Exiting.\n");
                return 1;
            }
            break;
    }

    tk_api_initialize();

    tk_api_Interface *TerminalInterface_p = tk_api_createTerminalInterface();

    if (!TerminalInterface_p) {
        puts("TerminalInterface creation failed.");
        return 1;
    }

    Session_p = tk_api_openSession(NULL, TerminalInterface_p, 1);
    if (!Session_p) {return 1;}

    nh_api_registerConfig("/etc/termoskanne.conf", 21);

//    if (Args.stdio) {
//        if (tk_api_claimStandardIO(TY_p)) {return 1;}
//    } else {
        tk_api_Renderer *Renderer_p = tk_api_openRenderer(NULL, Session_p);
        if (!Renderer_p) {return 1;}

        nh_api_Window *Window_p = 
            nh_api_createWindow(NULL, nh_api_getSurfaceRequirements());
        if (!Window_p) {return 1;}

        nh_api_Surface *Surface_p = nh_api_createSurface(Window_p);
        if (!Surface_p) {return 1;}

        Viewport_p = nh_api_createViewport(Surface_p, NULL, NULL);
        if (!Viewport_p) {return 1;}

        if (tk_api_setViewport(Renderer_p, Viewport_p) != TK_API_SUCCESS) {
            return 1;
        }

        nh_api_setWindowEventListener(Window_p, handleInput);
//    }

    while (nh_api_getWorkload(Session_p)) {
        if (!nh_api_run()) {usleep(10000);} // 10 milliseconds
    }

    nh_api_terminate();
}
