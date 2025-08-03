#ifndef TK_API_TK_CORE_H
#define TK_API_TK_CORE_H

// INCLUDES ========================================================================================

#include "nh-api/nh-api.h"
#include <stdbool.h>

// ENUMS ===========================================================================================

typedef enum TK_CORE_RESULT {
    TK_CORE_SUCCESS,
    TK_CORE_ERROR_NULL_POINTER,         
    TK_CORE_ERROR_BAD_STATE, 
    TK_CORE_ERROR_MEMORY_ALLOCATION,
    TK_CORE_ERROR_TERMINFO_DATA_CANNOT_BE_FOUND,
    TK_CORE_ERROR_UNKNOWN_TERMINAL_TYPE,
    TK_CORE_ERROR_TERMINAL_IS_HARDCOPY,
    TK_CORE_ERROR_UNKNOWN_COMMAND,
    TK_CORE_ERROR_INVALID_ARGUMENT,
} TK_CORE_RESULT;

typedef enum TK_CORE_PROGRAM_E {
    TK_CORE_PROGRAM_SHELL,
    TK_CORE_PROGRAM_LOGGER,
    TK_CORE_PROGRAM_EDITOR,
    TK_CORE_PROGRAM_TAGGER,
} TK_CORE_PROGRAM_E;

typedef enum TK_CORE_MARK_E {
    TK_CORE_MARK_LINE_VERTICAL     = 1 << 0, // Internal use.
    TK_CORE_MARK_LINE_HORIZONTAL   = 1 << 1, // Internal use.
    TK_CORE_MARK_LINE_GRAPHICS     = 1 << 2,
    TK_CORE_MARK_ELEVATED          = 1 << 3,
    TK_CORE_MARK_ACCENT            = 1 << 4,
    TK_CORE_MARK_ACCENT_BACKGROUND = 1 << 5,
} TK_CORE_MARK_E;

// TYPEDEFS ========================================================================================

typedef struct tk_core_TTY tk_core_TTY;
typedef struct tk_core_Program tk_core_Program;
typedef struct tk_core_Interface tk_core_Interface;
typedef struct tk_core_Glyph tk_core_Glyph;

typedef void *(*tk_core_init_f)(void *arg_p);
typedef TK_CORE_RESULT (*tk_core_draw_f)(tk_core_Program *Program_p, tk_core_Glyph *Glyphs_p, int width, int height, int row);
typedef TK_CORE_RESULT (*tk_core_drawTopbar_f)(tk_core_Program *Program_p, tk_core_Glyph *Glyphs_p, int width);
typedef TK_CORE_RESULT (*tk_core_getTitle_f)(tk_core_Program *Program_p, NH_API_UTF32 *title_p, int length);
typedef TK_CORE_RESULT (*tk_core_getCursorPosition_f)(tk_core_Program *Program_p, int *x_p, int *y_p);
typedef TK_CORE_RESULT (*tk_core_handleInput_f)(tk_core_Program *Program_p, nh_api_WSIEvent Event);
typedef TK_CORE_RESULT (*tk_core_update_f)(tk_core_Program *Program_p);
typedef TK_CORE_RESULT (*tk_core_handleCommand_f)(tk_core_Program *Program_p);
typedef void (*tk_core_destroy_f)(void *p);
typedef void (*tk_core_destroyPrototype_f)(tk_core_Interface *Prototype_p);

// STRUCTS =========================================================================================

typedef struct tk_core_GlyphAttributes {
    bool bold;
    bool faint;
    bool italic;
    bool underline;
    bool blink;
    bool reverse;
    bool invisible;
    bool struck;
    bool wrap;
    bool wide;
} tk_core_GlyphAttributes;

typedef struct tk_core_Color { 
    float r, g, b, a; 
} tk_core_Color; 

typedef struct tk_core_GlyphColor {
    tk_core_Color Color;
    bool custom;
} tk_core_GlyphColor;

typedef struct tk_core_Glyph {
    tk_core_GlyphAttributes Attributes;
    tk_core_GlyphColor Foreground;
    tk_core_GlyphColor Background;
    tk_core_GlyphColor Overlay;
    NH_API_UTF32 codepoint;
    NH_API_UTF32 overlay;
    TK_CORE_MARK_E mark;
} tk_core_Glyph;

typedef struct tk_core_Row {
    tk_core_Glyph *Glyphs_p;
    bool *update_p;
} tk_core_Row;

typedef struct tk_core_InterfaceCallbacks {
    tk_core_init_f init_f;
    tk_core_draw_f draw_f;
    tk_core_drawTopbar_f drawTopbar_f;
    tk_core_getTitle_f getTitle_f;
    tk_core_handleInput_f handleInput_f;
    tk_core_getCursorPosition_f getCursorPosition_f;
    tk_core_update_f update_f;
    tk_core_handleCommand_f handleCommand_f;
    tk_core_destroy_f destroy_f;
    tk_core_destroyPrototype_f destroyPrototype_f;
} tk_core_InterfaceCallbacks;

typedef struct tk_core_Interface {
    NH_API_UTF32 name_p[64];
    NH_API_UTF32 **commands_pp;
    unsigned int commands;
    tk_core_InterfaceCallbacks Callbacks;
    void *initArg_p;
} tk_core_Interface;

typedef struct tk_core_Program {
    tk_core_Interface *Prototype_p;
    void *handle_p;
    bool refresh;
    bool close;
    bool once;
    int command;
} tk_core_Program;

// FUNCTIONS =======================================================================================

tk_core_TTY *tk_api_openTTY(
    char *config_p, tk_core_Interface *Interface_p
);

TK_CORE_RESULT tk_api_closeTTY(
    tk_core_TTY *TTY_p
);

TK_CORE_RESULT tk_api_claimStandardIO(
    tk_core_TTY *TTY_p
);

TK_CORE_RESULT tk_api_unclaimStandardIO(
    tk_core_TTY *TTY_p
);

TK_CORE_RESULT tk_api_sendEvent(
    tk_core_TTY *TTY_p, nh_api_WSIEvent Event
);

#endif // TK_API_TK_CORE_H
