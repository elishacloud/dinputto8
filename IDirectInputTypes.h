#pragma once

DWORD ConvertDevTypeTo7(DWORD dwDevType, WORD wUsagePage, WORD wUsage, BOOL isHID, BOOL& IsGamepad);
DWORD ConvertDevSubTypeTo7(DWORD dwDevType, DWORD dwDevType7, DWORD dwDevSubType, BOOL IsGamepad);
