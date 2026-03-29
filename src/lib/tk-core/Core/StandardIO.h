#ifndef TK_CORE_STANDARD_IO_H
#define TK_CORE_STANDARD_IO_H

#include "Session.h"
#include "View.h"
#include "../Common/Includes.h"

TK_API_RESULT tk_core_getStandardOutputWindowSize(
    int *cols_p, int *rows_p
);

TK_API_RESULT tk_core_readStandardInput(
    tk_core_Session *Session_p
);

TK_API_RESULT tk_core_writeCursorToStandardOutput(
    int x, int y
);

TK_API_RESULT tk_core_writeToStandardOutput(
    tk_core_Row *Rows_p, int cols, int rows
);

TK_API_RESULT tk_core_claimStandardIO(
    tk_core_Session *Session_p
);

TK_API_RESULT tk_core_unclaimStandardIO(
    tk_core_Session *Session_p
);

bool tk_core_claimsStandardIO(
    tk_core_Session *Session_p
);

#endif 
