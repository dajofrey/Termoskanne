// LICENSE NOTICE ==================================================================================

/**
 * TTýr - Terminal Emulator
 * Copyright (C) 2022  Dajo Frey
 * Published under GNU LGPL. See TTyr/LICENSE.LGPL file.
 */

// INCLUDES ========================================================================================

#include "Program.h"
#include "Session.h"

#include "../Editor/Editor.h"
#include "../Shell/Shell.h"
#include "../Common/Macros.h"

#include "nh-core/Loader/Loader.h"
#include "nh-core/System/Memory.h"
#include "nh-core/System/Thread.h"

#include "nh-encoding/Encodings/UTF32.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// PROGRAM INSTANCE ================================================================================

tk_core_Program *tk_core_createProgramInstance(
    tk_api_Interface *Interface_p, bool once)
{
    tk_core_Program *Program_p = (tk_core_Program*)nh_core_allocate(sizeof(tk_core_Program));
    TK_CHECK_MEM_2(NULL, Program_p)

    Program_p->once        = once;
    Program_p->refresh     = false;
    Program_p->close       = false;
    Program_p->Prototype_p = Interface_p;
    Program_p->handle_p    = Interface_p->Callbacks.init_f ? 
        Interface_p->Callbacks.init_f(Interface_p->initArg_p) : NULL; 
 
    return Program_p;
}

TK_API_RESULT tk_core_destroyProgramInstance(
    tk_core_Program *Program_p)
{
    if (Program_p->once) {
        Program_p->Prototype_p->Callbacks.destroyPrototype_f(Program_p->Prototype_p);
    }

    if (Program_p && Program_p->Prototype_p && Program_p->Prototype_p->Callbacks.destroy_f) {
        Program_p->Prototype_p->Callbacks.destroy_f(Program_p->handle_p);
    }

    nh_core_free(Program_p);
 
    return TK_API_SUCCESS;
}

// INTERFACE =======================================================================================

TK_API_RESULT tk_core_addProgram(
    tk_core_Session *Session_p, tk_api_Interface *Prototype_p, bool once)
{
//    if (once) {
//        tk_core_MacroTile *Tile_p = Session_p->Window_p->Tile_p;
//
//        // Remove program with same name on tile if necessary.
//        for (int i = 0; Tile_p->Canvas.Programs_p && i < Tile_p->Canvas.Programs_p->length; ++i) {
//            tk_core_Program *Program_p = ((tk_core_Program*)Tile_p->Canvas.Programs_p->p)+i;
//            if (ttyr_encoding_compareUTF32(Program_p->Prototype_p->Name.p, Prototype_p->Name.p)) {
//                TK_CHECK(tk_core_destroyProgramInstance(((tk_core_Program*)Tile_p->Canvas.Programs_p->p)+i))
//                nh_core_removeFromArray(Tile_p->Canvas.Programs_p, i, 1);
//                if (Program_p->once) {
//                    Program_p->Prototype_p->Callbacks.destroyPrototype_f(Program_p->Prototype_p);
//                }
//            }
//        }
//
//        // Add canvas tile to tile.
//        TK_CHECK(tk_core_appendCanvasTile(&Tile_p->Canvas, Prototype_p, &Session_p->Views, once))
//
//        // Switch to new program.
//        Tile_p->Canvas.currentProgram = Tile_p->Canvas.Programs_p->length-1; 
//
//        // Prevent black screen.
//        Session_p->Window_p->refreshScreen = true;
//
//        TK_CORE_END(TK_API_SUCCESS)
//    }

    // Check if there already exists a program with this name.
    for (int i = 0; i < Session_p->Prototypes.size; ++i) {
        if (nh_encoding_compareUTF32(((tk_api_Interface*)Session_p->Prototypes.pp[i])->name_p, Prototype_p->name_p)) {
            return TK_API_ERROR_BAD_STATE;
        }
    }

    // Add program prototype to other program prototypes.
    nh_core_appendToList(&Session_p->Prototypes, Prototype_p);

    // If necessary, Add program prototype instance to all current tiles.
    nh_core_List Tiles = tk_core_getTiles(Session_p->Window_p->RootTile_p);
    for (int i = 0; i < Tiles.size; ++i) {
        tk_core_Tile *Tile_p = Tiles.pp[i];
        bool add = true;
        for (int j = 0; TK_CORE_MACRO_TAB(Tile_p)->MicroWindow.Tabs_p && j < TK_CORE_MACRO_TAB(Tile_p)->MicroWindow.Tabs_p->size; ++j) {
            tk_api_Interface *MicroTileProgramPrototype_p = 
                ((tk_core_MicroTab*)TK_CORE_MACRO_TAB(Tile_p)->MicroWindow.Tabs_p->pp[j])->Prototype_p;
            if (!MicroTileProgramPrototype_p) {continue;}
            if (nh_encoding_compareUTF32(MicroTileProgramPrototype_p->name_p, Prototype_p->name_p)) {
                add = false;
            }
        }
        if (!add) {continue;}
        for (int j = 0; j < 9; ++j) {
            TK_CHECK(tk_core_appendMicroTab(
                &TK_CORE_MACRO_TAB_2(Tile_p, j)->MicroWindow, Session_p->Prototypes.pp[Session_p->Prototypes.size - 1], once
            ))
        }
    }
    nh_core_freeList(&Tiles, false);

    // Prevent black screen. 
    Session_p->Window_p->refreshGrid1 = true;

    return TK_API_SUCCESS;
}
