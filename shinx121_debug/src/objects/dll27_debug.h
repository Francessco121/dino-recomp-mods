#pragma once

#include "PR/ultratypes.h"
#include "dlls/engine/27.h"
#include "game/objects/object.h"

DLL27_Data *dll27_debug_get_data(Object *obj);
void dll27_debug_tab(Object *obj, DLL27_Data *data);
