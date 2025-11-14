#pragma once

#include "PR/ultratypes.h"
#include "dlls/engine/18_objfsa.h"
#include "game/objects/object.h"

ObjFSA_Data *objfsa_debug_get_data(Object *obj);
void objfsa_debug_tab(Object *obj, ObjFSA_Data *fsa);
