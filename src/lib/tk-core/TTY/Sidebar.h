#ifndef TK_CORE_SIDEBAR_H
#define TK_CORE_SIDEBAR_H

#include "TTY.h"
#include "../Common/Includes.h"

bool tk_core_handleSidebarMove(
    tk_core_TTY *TTY_p, int cCol, int cRow
);

void tk_core_handleSidebarHit(
    tk_core_TTY *TTY_p, nh_api_MouseEvent Event, int cRow
);

TK_CORE_RESULT tk_core_drawSidebar(
    tk_core_TTY *TTY_p
);

#endif 
