#pragma once

DWORD ConvertDevTypeTo7(DWORD dwDevType);
DWORD ConvertDevSubTypeTo7(DWORD dwDevType, DWORD dwDevSubType);
void ConvertEffect(DIEFFECT &eff1, const DIEFFECT &eff2);
