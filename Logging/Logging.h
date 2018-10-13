#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "External\Logging\Logging.h"

std::ostream& operator<<(std::ostream& os, REFIID riid);
