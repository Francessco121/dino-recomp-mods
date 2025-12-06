#pragma once

#include "object_editor.h"

#include "game/objects/object.h"

void object_edit_contents(Object *obj);
void object_seq_debug(Object *obj, ObjEditorData *editorData);
void object_anim_debug(Object *obj, ObjEditorData *editorData);
