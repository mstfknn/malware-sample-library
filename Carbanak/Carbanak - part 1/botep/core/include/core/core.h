#pragma once

#include "winapi.h"
#include "memory.h"
#include "string.h"
#include "process.h"
#include "path.h"

namespace Core
{

bool Init();
void Release();

}

bool InitBot();
void ReleaseBot();

