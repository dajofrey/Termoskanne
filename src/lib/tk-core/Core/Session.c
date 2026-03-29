// LICENSE NOTICE ==================================================================================

/**
 * Termoskanne - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// DEFINE ==========================================================================================

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

// INCLUDES ========================================================================================

#include "Session.h"
#include "View.h"
#include "StandardIO.h"
#include "Draw.h"
#include "Macro.h"
#include "ContextMenu.h"
#include "Titlebar.h"

#include "../Shell/Shell.h"
#include "../Common/Macros.h"

#include "nh-core/System/Thread.h"
#include "nh-core/System/Memory.h"
#include "nh-core/System/Process.h"
#include "nh-core/Util/RingBuffer.h"

#include "nh-encoding/Base/UnicodeDataHelper.h"
#include "nh-encoding/Encodings/UTF8.h"
#include "nh-encoding/Encodings/UTF32.h"

#include <string.h>
#include <stdio.h>

// CLIPBOARD =======================================================================================

TK_API_RESULT tk_core_resetClipboard()
{
    tk_core_Session *Session_p = nh_core_getWorkloadArg();

    for (int i = 0; i < Session_p->Clipboard.Lines.length; ++i) {
        nh_encoding_UTF32String *String_p = 
            &((nh_encoding_UTF32String*)Session_p->Clipboard.Lines.p)[i];
        nh_encoding_freeUTF32(String_p);
    }

    nh_core_freeArray(&Session_p->Clipboard.Lines);
    Session_p->Clipboard.Lines = nh_core_initArray(sizeof(nh_encoding_UTF32String), 32);

    return TK_API_SUCCESS;
}

nh_encoding_UTF32String *tk_core_newClipboardLine()
{
    tk_core_Session *Session_p = nh_core_getWorkloadArg();

    nh_encoding_UTF32String *Line_p = (nh_encoding_UTF32String*)nh_core_incrementArray(&Session_p->Clipboard.Lines);

    TK_CHECK_NULL_2(NULL, Line_p)
    *Line_p = nh_encoding_initUTF32(32);

    return Line_p;
}

tk_core_Clipboard *tk_core_getClipboard()
{
    return &((tk_core_Session*)nh_core_getWorkloadArg())->Clipboard;
}

// INIT/FREE =======================================================================================
// The next functions comprise the in/exit points of nhtty.

typedef struct tk_core_OpenTTY {
    char *config_p;
    tk_core_Interface *Interface_p;
} tk_core_OpenTTY;

static void *tk_core_initTTY(
    nh_core_Workload *Workload_p)
{
    tk_core_OpenTTY *Args_p = Workload_p->args_p;

    static char *path_p = "tk-core/Core/Core.c";
    static char *name_p = "tk-core Workload";
    Workload_p->path_p = path_p;
    Workload_p->name_p = name_p;
    Workload_p->module = -1;

    tk_core_Session *Session_p = (tk_core_Session*)nh_core_allocate(sizeof(tk_core_Session));
    TK_CHECK_MEM_2(NULL, Session_p)

    memset(Session_p, 0, sizeof(tk_core_Session));

    tk_core_updateConfig(Session_p);

    if (Workload_p->args_p) {
        strcpy(Session_p->namespace_p, Workload_p->args_p);
    } else {
        sprintf(Session_p->namespace_p, "%p", Session_p);
    }

    Session_p->Windows = nh_core_initList(10);
    Session_p->Views = nh_core_initList(8);
    Session_p->Prototypes = nh_core_initList(8);
    Session_p->Clipboard.Lines = nh_core_initArray(sizeof(nh_encoding_UTF32String), 32);

    Session_p->hasFocus = true;
    Session_p->Borders.on = true;
    Session_p->Topbars.on = true;
    Session_p->Sidebar.hover = -999;
    Session_p->Titlebar.hover = -1;

    Session_p->Preview.blink = true;
    Session_p->Preview.LastBlink = nh_core_getSystemTime();

    Session_p->Renderer_p = NULL;

    TK_CHECK_2(NULL, nh_core_initRingBuffer(
        &Session_p->Events, 128, sizeof(nh_api_WSIEvent), NULL 
    ))

    TK_CHECK_NULL_2(NULL, tk_core_insertAndFocusWindow(Session_p, 0))

    if (Args_p->Interface_p == NULL) {
        Args_p->Interface_p = tk_core_createShellInterface();
    }
    tk_core_addProgram(Session_p, Args_p->Interface_p, false);
 
    return Session_p;
}

static void tk_core_freeTTY(
    void *p)
{
    tk_core_Session *Session_p = p;

    tk_core_destroyWindows(Session_p);
 
    nh_core_freeArray(&(Session_p->Clipboard.Lines));
    nh_core_freeRingBuffer(&(Session_p->Events));

    if (tk_core_claimsStandardIO(Session_p)) {
        tk_core_unclaimStandardIO(Session_p);
    }

    for (int i = 0; i < Session_p->Prototypes.size; ++i) {
        if (((tk_core_Interface*)Session_p->Prototypes.pp[i])->Callbacks.destroyPrototype_f) {
            ((tk_core_Interface*)Session_p->Prototypes.pp[i])->Callbacks.destroyPrototype_f(Session_p->Prototypes.pp[i]);
        }
    }
    nh_core_freeList(&Session_p->Prototypes, false);

    for (int i = 0; i < Session_p->Views.size; ++i) {
        tk_core_destroyView(Session_p, Session_p->Views.pp[i]);
    }
    nh_core_freeList(&Session_p->Views, false);

    nh_core_free(Session_p);
}

// RUN LOOP ========================================================================================
// The next functions comprise the top-level of the nhtty run loop.

static TK_API_RESULT tk_core_handleInput(
    tk_core_Session *Session_p)
{
    if (tk_core_claimsStandardIO(Session_p)) {
        TK_CHECK(tk_core_readStandardInput(Session_p))
    }

    while (1)
    {
        nh_api_WSIEvent *Event_p =
            (nh_api_WSIEvent*)nh_core_incrementRingBufferMarker(&Session_p->Events, &Session_p->Events.Marker);

        if (Event_p == NULL) {break;}

        TK_CHECK(tk_core_handleMacroWindowInput(&Session_p->Config, Session_p->Window_p, *Event_p))

        for (int i = 0; i < Session_p->Views.size; ++i) {
            TK_CHECK(tk_core_forwardEvent(Session_p->Views.pp[i], *Event_p))
        }
    }

    return TK_API_SUCCESS;
}

static TK_API_RESULT tk_core_handleWindowResize(
    tk_core_Session *Session_p, tk_core_View *View_p)
{
    bool updated = false;

    TK_CHECK(tk_core_getViewSize(View_p))
    TK_CHECK(tk_core_updateView(&Session_p->Config, View_p, &updated, true))

    if (updated) {
        if (Session_p->Window_p->MouseMenu_p) {
            Session_p->Window_p->MouseMenu_p = NULL;
        }

	Session_p->Window_p->refreshGrid1 = true;
	Session_p->Window_p->refreshGrid2 = true;
	Session_p->Window_p->refreshTitlebar = true;
    }

    return TK_API_SUCCESS;
}

static NH_SIGNAL tk_core_runTTY(
    void *p)
{
    tk_core_Session *Session_p = p;
    bool idle = true;

    tk_core_updateConfig(Session_p);

    for (int i = 0; i < Session_p->Views.size; ++i) {
        TK_CHECK_2(NH_SIGNAL_ERROR, tk_core_handleWindowResize(Session_p, Session_p->Views.pp[i]))
    }
    if (Session_p->Views.size <= 0 || ((tk_core_View*)Session_p->Views.pp[0])->cols <= 0) {return NH_SIGNAL_IDLE;}

    TK_CHECK_2(NH_SIGNAL_ERROR, tk_core_handleInput(Session_p))
    TK_CHECK_2(NH_SIGNAL_ERROR, tk_core_updateMacroWindow(Session_p->Window_p))
    tk_core_checkTitlebar(&Session_p->Config, &Session_p->Titlebar, &Session_p->Window_p->refreshGrid1);

    if (Session_p->Window_p->refreshGrid1) {
        TK_CHECK_2(NH_SIGNAL_ERROR, tk_core_drawTitlebar(Session_p))
        TK_CHECK_2(NH_SIGNAL_ERROR, tk_core_refreshGrid1(Session_p))
    }
    if (Session_p->Window_p->refreshGrid2) {
        TK_CHECK_2(NH_SIGNAL_ERROR, tk_core_refreshGrid2(Session_p))
    }

    if (Session_p->Window_p->refreshCursor || Session_p->Window_p->refreshGrid1) {
        TK_CHECK_2(NH_SIGNAL_ERROR, tk_core_refreshCursor(Session_p))
        idle = false;
    }

    Session_p->Window_p->refreshGrid1 = false;
    Session_p->Window_p->refreshGrid2 = false;
    Session_p->Window_p->refreshCursor = false;
 
    if (Session_p->Window_p->close) {
        // First, close all dependent workloads.
        for (int i = 0; i < Session_p->Views.size; ++i) {
            tk_core_View *View_p = Session_p->Views.pp[i];
            nh_core_Workload *Workload_p = nh_core_getWorkload(View_p->p);
            if (Workload_p) {
                nh_core_deactivateWorkload(Workload_p);
            }
        }
        return NH_SIGNAL_FINISH;
    }

    return idle ? NH_SIGNAL_IDLE : NH_SIGNAL_OK;
}

// COMMANDS ========================================================================================
// The next functions are executed by tk_core_cmd_* functions.

typedef enum TK_CORE_COMMAND_E {
    TK_CORE_COMMAND_CLAIM_STANDARD_IO,
    TK_CORE_COMMAND_UNCLAIM_STANDARD_IO,
    TK_CORE_COMMAND_SEND_EVENT,
} TK_CORE_COMMAND_E;

typedef struct tk_core_AddProgramData {
    char name_p[255];
} tk_core_AddProgramData;

/**
 * Here, most commands that came in through the API are handled.
 */
static NH_SIGNAL tk_core_runTTYCommand(
    void *p, nh_core_WorkloadCommand *Command_p)
{
    tk_core_Session *Session_p = p;

    switch (Command_p->type)
    {
        case TK_CORE_COMMAND_CLAIM_STANDARD_IO :
            TK_CHECK(tk_core_claimStandardIO(Session_p))
            break;
        case TK_CORE_COMMAND_UNCLAIM_STANDARD_IO :
            TK_CHECK(tk_core_unclaimStandardIO(Session_p))
            break;
        case TK_CORE_COMMAND_SEND_EVENT :
        {
            nh_api_WSIEvent *Event2_p = (nh_api_WSIEvent*)nh_core_advanceRingBuffer(&Session_p->Events);
            TK_CHECK_NULL(Event2_p)
            *Event2_p = *((nh_api_WSIEvent*)Command_p->p);
            break;
        }
    }

    return NH_SIGNAL_OK;
}

// API =============================================================================================
// The next functions are called by lib/netzhaut/nhtty.h functions.

tk_core_Session *tk_core_openSession(
    char *config_p, tk_core_Interface *Interface_p)
{
    tk_core_OpenTTY OpenTTY;
    OpenTTY.config_p = config_p;
    OpenTTY.Interface_p = Interface_p;

    tk_core_Session *Session_p = nh_core_activateWorkload(
        tk_core_initTTY, tk_core_runTTY, tk_core_freeTTY, tk_core_runTTYCommand, &OpenTTY, true
    );

    return Session_p;
}

TK_API_RESULT tk_core_closeSession(
    tk_core_Session *Session_p)
{
    nh_core_deactivateWorkload(nh_core_getWorkload(Session_p));
    return TK_API_SUCCESS;
}

TK_API_RESULT tk_core_cmd_claimStandardIO(
    tk_core_Session *Session_p)
{
    nh_core_executeWorkloadCommand(Session_p, TK_CORE_COMMAND_CLAIM_STANDARD_IO, NULL, 0);
    return TK_API_SUCCESS;
}

TK_API_RESULT tk_core_cmd_unclaimStandardIO(
    tk_core_Session *Session_p)
{
    nh_core_executeWorkloadCommand(Session_p, TK_CORE_COMMAND_UNCLAIM_STANDARD_IO, NULL, 0);
    return TK_API_SUCCESS;
}

TK_API_RESULT tk_core_cmd_sendEvent(
    tk_core_Session *Session_p, nh_api_WSIEvent Event)
{
    switch (Event.type) {
        case NH_API_WSI_EVENT_KEYBOARD :
        case NH_API_WSI_EVENT_MOUSE :
        case NH_API_WSI_EVENT_WINDOW :
            nh_core_executeWorkloadCommand(Session_p, TK_CORE_COMMAND_SEND_EVENT, &Event, sizeof(nh_api_WSIEvent));
        default :
            break;
            // Ignore other events.
    }

    return TK_API_SUCCESS;
}
