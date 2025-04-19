#include "modding.h"
#include "imports.h"
#include "common.h"
#include "sys/linked_list.h"

#include "3d.h"

extern Object** get_world_objects(s32*, s32 *count);
extern LinkedList gObjUpdateList;

static s32 object_debug_window_open = FALSE;
static s32 hovered_object_idx = -1;
static s32 show_in_world = FALSE;
static s32 show_hitboxes = TRUE;
static char search_buffer[256] = {0};
static Object *edit_objects[256] = {NULL};

static u16 resolve_dll_number(u16 id) {
    if (id >= 0x8000) {
        id -= 0x8000;
        // bank3
        id += gFile_DLLS_TAB->header.bank3;
    } else if (id >= 0x2000) {
        id -= 0x2000;
        // bank2
        id += gFile_DLLS_TAB->header.bank2 + 1;
    } else if (id >= 0x1000) {
        id -= 0x1000;
        // bank1
        id += gFile_DLLS_TAB->header.bank1 + 1;
    }

    return id;
}

static void add_edit_object(Object *obj) {
    for (s32 i = 0; i < (s32)(sizeof(edit_objects) / sizeof(Object*)); i++) {
        if (edit_objects[i] == obj) {
            return;
        }
    }

    for (s32 i = 0; i < (s32)(sizeof(edit_objects) / sizeof(Object*)); i++) {
        if (edit_objects[i] == NULL) {
            edit_objects[i] = obj;
            break;
        }
    }
}

static void remove_freed_edit_objects() {
    s32 _, count;
    Object** objects = get_world_objects(&_, &count);

    for (s32 i = 0; i < (s32)(sizeof(edit_objects) / sizeof(Object*)); i++) {
        Object *obj = edit_objects[i];
        if (edit_objects[i] != NULL) {
            s32 found = FALSE;
            for (s32 k = 0; k < count; k++) {
                if (objects[k] == obj) {
                    found = TRUE;
                    break;
                }
            }

            if (!found) {
                edit_objects[i] = NULL;
            }
        }
    }
}

static void dbgui_input_byte(const char *label, u8 *value) {
    s32 value_ = *value;
    if (recomp_dbgui_input_int(label, &value_)) {
        *value = (u8)value_;
    }
}

static s32 dbgui_input_sbyte(const char *label, s8 *value) {
    s32 value_ = *value;
    if (recomp_dbgui_input_int(label, &value_)) {
        *value = (s8)value_;
        return TRUE;
    }

    return FALSE;
}

static void dbgui_input_short(const char *label, s16 *value) {
    s32 value_ = *value;
    if (recomp_dbgui_input_int(label, &value_)) {
        *value = (s16)value_;
    }
}

static void dbgui_input_uint(const char *label, u32 *value) {
    s32 value_ = *value;
    if (recomp_dbgui_input_int(label, &value_)) {
        *value = (u32)value_;
    }
}

static void dbgui_input_vec3f(const char *label, Vec3f *value) {
    Vec3f value_ = *value;
    s32 changed = FALSE;
    recomp_dbgui_push_str_id(label);

    recomp_dbgui_text(label);
    recomp_dbgui_same_line();
    recomp_dbgui_set_next_item_width(80);
    if (recomp_dbgui_input_float("x", &value_.x)) {
        changed = TRUE;
    }
    recomp_dbgui_same_line();
    recomp_dbgui_set_next_item_width(80);
    if (recomp_dbgui_input_float("y", &value_.y)) {
        changed = TRUE;
    }
    recomp_dbgui_same_line();
    recomp_dbgui_set_next_item_width(80);
    if (recomp_dbgui_input_float("z", &value_.z)) {
        changed = TRUE;
    }

    if (changed) {
        *value = value_;
    }

    recomp_dbgui_pop_id();
}

static void dbgui_input_obj_position(const char *label, Object *value) {
    Vec3f value_ = value->srt.transl;

    s32 changed = FALSE;
    recomp_dbgui_push_str_id(label);

    recomp_dbgui_text(label);
    recomp_dbgui_same_line();
    recomp_dbgui_set_next_item_width(80);
    if (recomp_dbgui_input_float("x", &value_.x)) {
        changed = TRUE;
    }
    recomp_dbgui_same_line();
    recomp_dbgui_set_next_item_width(80);
    if (recomp_dbgui_input_float("y", &value_.y)) {
        changed = TRUE;
    }
    recomp_dbgui_same_line();
    recomp_dbgui_set_next_item_width(80);
    if (recomp_dbgui_input_float("z", &value_.z)) {
        changed = TRUE;
    }

    if (changed) {
        value->srt.transl = value_;
        value->positionMirror = value_;
        value->positionMirror2 = value_;
        value->positionMirror3 = value_;
    }

    recomp_dbgui_pop_id();
}

static void object_edit_contents(Object *obj) {
    recomp_dbgui_textf("Position: %f,%f,%f",
        obj->srt.transl.x, obj->srt.transl.y, obj->srt.transl.z);
    recomp_dbgui_textf("Rotation (y,p,r): %d,%d,%d", 
        obj->srt.yaw, obj->srt.pitch, obj->srt.roll);
    recomp_dbgui_input_float("Scale", &obj->srt.scale);
    dbgui_input_short("Flags", &obj->srt.flags);

    recomp_dbgui_textf("Speed: %f,%f,%f", 
        obj->speed.x, obj->speed.y, obj->speed.z);

    if (obj->parent != NULL) {
        if (recomp_dbgui_tree_node("parent")) {
            object_edit_contents(obj->parent);
            recomp_dbgui_tree_pop();
        }
    } else {
        recomp_dbgui_textf("parent: null");
    }
    
    dbgui_input_byte("unk_0x34", &obj->unk_0x34);
    dbgui_input_sbyte("matrixIdx", &obj->matrixIdx);
    dbgui_input_byte("unk_0x36", &obj->unk_0x36);
    dbgui_input_byte("unk_0x37", &obj->unk_0x37);
    recomp_dbgui_textf("next: %p", &obj->next);
    recomp_dbgui_textf("unk0x3c: %f", obj->unk0x3c);
    recomp_dbgui_textf("unk0x40: %f", obj->unk0x40);
    recomp_dbgui_textf("group: %u", obj->group);
    recomp_dbgui_textf("id: %u", obj->id);
    recomp_dbgui_textf("tabIdx: %u", obj->tabIdx);
    if (obj->createInfo != NULL) {
        if (recomp_dbgui_tree_node("createInfo")) {
            ObjCreateInfo *createInfo = obj->createInfo;
            recomp_dbgui_textf("objId: %d", createInfo->objId);
            recomp_dbgui_textf("unk2: %d", createInfo->unk2);
            recomp_dbgui_textf("unk3: %d", createInfo->unk3);
            recomp_dbgui_textf("unk4: %d", createInfo->unk4);
            recomp_dbgui_textf("unk5: %d", createInfo->unk5);
            recomp_dbgui_textf("unk6: %d", createInfo->unk6);
            recomp_dbgui_textf("unk7: %d", createInfo->unk7);
            recomp_dbgui_textf("x: %f", createInfo->x);
            recomp_dbgui_textf("y: %f", createInfo->y);
            recomp_dbgui_textf("z: %f", createInfo->z);
            recomp_dbgui_tree_pop();
        }
    } else {
        recomp_dbgui_textf("createInfo: null");
    }
    if (obj->def != NULL) {
        if (recomp_dbgui_tree_node("def")) {
            ObjDef *def = obj->def;
            recomp_dbgui_textf("dll: %d", resolve_dll_number(def->dllID));
            recomp_dbgui_textf("numModels: %d", def->numModels);
            recomp_dbgui_textf("name: %s", def->name);

            if (def->pAttachPoints != NULL) {
                if (recomp_dbgui_tree_node("pAttachPoints")) {
                    for (int i = 0; i < def->numAttachPoints; i++) {
                        if (recomp_dbgui_tree_node(recomp_sprintf_helper("[%d]", i))) {
                            recomp_dbgui_input_float("x", &def->pAttachPoints[i].pos.x);
                            recomp_dbgui_input_float("y", &def->pAttachPoints[i].pos.y);
                            recomp_dbgui_input_float("z", &def->pAttachPoints[i].pos.z);

                            recomp_dbgui_tree_pop();
                        }
                    }

                    recomp_dbgui_tree_pop();
                }
            }

            recomp_dbgui_tree_pop();
        }
    } else {
        recomp_dbgui_textf("def: null");
    }
    if (obj->objhitInfo != NULL) {
        if (recomp_dbgui_tree_node("objhitInfo")) {
            ObjectHitInfo *objhitInfo = obj->objhitInfo;
            recomp_dbgui_textf("unk_0x10: %f,%f,%f", 
                objhitInfo->unk_0x10.x, objhitInfo->unk_0x10.y, objhitInfo->unk_0x10.z);
            recomp_dbgui_textf("unk_0x20: %f,%f,%f", 
                objhitInfo->unk_0x20.x, objhitInfo->unk_0x20.y, objhitInfo->unk_0x20.z);
            recomp_dbgui_textf("unk_0x40: %d", objhitInfo->unk_0x40);
            recomp_dbgui_textf("unk_0x44: %d", objhitInfo->unk_0x44);
            recomp_dbgui_textf("unk_0x48: %d", objhitInfo->unk_0x48);
            recomp_dbgui_textf("unk_0x4c: %d", objhitInfo->unk_0x4c);
            recomp_dbgui_textf("unk_0x52: %d", objhitInfo->unk_0x52);
            recomp_dbgui_textf("unk_0x54: %d", objhitInfo->unk_0x54);
            recomp_dbgui_textf("unk_0x56: %d", objhitInfo->unk_0x56);
            recomp_dbgui_textf("unk_0x58: %d", objhitInfo->unk_0x58);
            recomp_dbgui_textf("unk_0x5a: %d", objhitInfo->unk_0x5a);
            recomp_dbgui_textf("unk_0x5f: %d", objhitInfo->unk_0x5f);
            recomp_dbgui_textf("unk_0x60: %d", objhitInfo->unk_0x60);
            recomp_dbgui_textf("unk_0x61: %d", objhitInfo->unk_0x61);
            recomp_dbgui_textf("unk_0x62: %d", objhitInfo->unk_0x62);
            recomp_dbgui_textf("unk_0x9e: %d", objhitInfo->unk_0x9e);
            recomp_dbgui_textf("unk_0x9f: %d", objhitInfo->unk_0x9f);
            recomp_dbgui_tree_pop();
        }
    } else {
        recomp_dbgui_textf("objhitInfo: null");
    }
    recomp_dbgui_textf("unk0x58: %d", obj->unk0x58);
    recomp_dbgui_textf("ptr0x5c: %p", obj->ptr0x5c);
    recomp_dbgui_textf("ptr0x60: %p", obj->ptr0x60);
    recomp_dbgui_textf("ptr0x64: %p", obj->ptr0x64);

    recomp_dbgui_textf("dll: %p", obj->dll);
    recomp_dbgui_textf("ptr0x6c: %p", obj->ptr0x6c);
    recomp_dbgui_textf("ptr0x70: %p", obj->ptr0x70);
    recomp_dbgui_textf("unk0x74: %d", obj->unk0x74);
    if (obj->unk_0x78 != NULL) {
        if (recomp_dbgui_tree_node("unk_0x78")) {
            ObjectStruct78 *unk_0x78 = obj->unk_0x78;
            recomp_dbgui_textf("unk0: %u", unk_0x78->unk0);
            recomp_dbgui_textf("unk1: %u", unk_0x78->unk1);
            recomp_dbgui_textf("unk2: %u", unk_0x78->unk2);
            recomp_dbgui_textf("unk3: %u", unk_0x78->unk3);
            recomp_dbgui_textf("unk4: %u", unk_0x78->unk4);
            recomp_dbgui_tree_pop();
        }
    } else {
        recomp_dbgui_textf("unk_0x78: null");
    }
    recomp_dbgui_textf("positionMirror2: %f,%f,%f",
        obj->positionMirror2.x, obj->positionMirror2.y, obj->positionMirror2.z);
    recomp_dbgui_textf("positionMirror3: %f,%f,%f",
        obj->positionMirror3.x, obj->positionMirror3.y, obj->positionMirror3.z);
    recomp_dbgui_textf("animTimer: %f", obj->animTimer);
    recomp_dbgui_textf("unk0x9c: %f", obj->unk0x9c);
    recomp_dbgui_textf("curAnimId: %d", obj->curAnimId);
    recomp_dbgui_textf("unk_0xa2: %d", obj->unk_0xa2);
    recomp_dbgui_textf("unk_0xa4: %f", obj->unk_0xa4);
    recomp_dbgui_textf("unk_0xa8: %f", obj->unk_0xa8);
    recomp_dbgui_textf("mapID: %d", obj->mapID);
    if (dbgui_input_sbyte("modelInstIdx", &obj->modelInstIdx)) {
        // Game will crash if this goes out of bounds
        if (obj->modelInstIdx < 0) {
            obj->modelInstIdx = 0;
        } else if (obj->modelInstIdx >= obj->def->numModels) {
            obj->modelInstIdx = obj->def->numModels - 1;
        }
    }
    recomp_dbgui_textf("updatePriority: %d", obj->updatePriority);
    recomp_dbgui_textf("unk0xaf: %d", obj->unk0xaf);
    recomp_dbgui_textf("unk0xb0: %u", obj->unk0xb0);
    recomp_dbgui_textf("unk0xb2: %u", obj->unk0xb2);
    recomp_dbgui_textf("unk0xb4: %u", obj->unk0xb4);
    recomp_dbgui_textf("state: %p", obj->state);
    recomp_dbgui_textf("unk0xbc: %p", obj->unk0xbc);
    if (obj->unk0xc0 != NULL) {
        if (recomp_dbgui_tree_node("unk0xc0")) {
            object_edit_contents(obj->unk0xc0);
            recomp_dbgui_tree_pop();
        }
    } else {
        recomp_dbgui_textf("unk0xc0: null");
    }
    recomp_dbgui_textf("unk0xc4: %u", obj->unk0xc4);
    if (obj->linkedObject != NULL) {
        if (recomp_dbgui_tree_node("linkedObject")) {
            object_edit_contents(obj->linkedObject);
            recomp_dbgui_tree_pop();
        }
    } else {
        recomp_dbgui_textf("linkedObject: null");
    }
    recomp_dbgui_textf("ptr0xcc: %p", obj->ptr0xcc);
    recomp_dbgui_textf("unk_0xd4: %u", obj->unk_0xd4);
    recomp_dbgui_textf("unk_0xd6: %u", obj->unk_0xd6);
    recomp_dbgui_textf("unk_0xd8: %u", obj->unk_0xd8);
    recomp_dbgui_textf("unk_0xd9: %u", obj->unk_0xd9);
    recomp_dbgui_textf("unk_0xda: %u", obj->unk_0xda);
    recomp_dbgui_textf("unk0xdc: %d", obj->unk0xdc);
}

static void object_editor(Object *obj, s32 index) {
    s32 open = TRUE;
    if (recomp_dbgui_begin(recomp_sprintf_helper("%s###object_edit_%p", obj->def->name, obj), &open)) {
        object_edit_contents(obj);
    }
    recomp_dbgui_end();

    if (!open) {
        edit_objects[index] = NULL;
    }
}

static void objects_list_tab(Object** objects, s32 count, s32 *hovered_object_idx) {
    recomp_dbgui_checkbox("Show in world", &show_in_world);

    if (show_in_world) {
        recomp_dbgui_checkbox("Show 3D boxes", &show_hitboxes);
    }

    recomp_dbgui_input_text("Search", search_buffer, sizeof(search_buffer) / sizeof(char));
    s32 search_len = strlen(search_buffer);

    recomp_dbgui_textf("Objects (%d):", count);
    if (recomp_dbgui_begin_child("object_list")) {
        for (s32 i = 0; i < count; i++) {
            Object *obj = objects[i];
            char *name = obj->def->name;

            if (search_len != 0) {
                s32 skip = FALSE;
                for (s32 k = 0; k < search_len && k < 13; k++) {
                    if (name[k] != search_buffer[k]) {
                        skip = TRUE;
                        break;
                    }
                }

                if (skip) {
                    continue;
                }
            }

            recomp_dbgui_push_str_id(recomp_sprintf_helper("%p", obj));

            if (recomp_dbgui_button("Edit")) {
                add_edit_object(obj);
            }

            recomp_dbgui_same_line();

            recomp_dbgui_textf("[%d] %s", i, name);

            if (recomp_dbgui_is_item_hovered()) {
                *hovered_object_idx = i;
            }

            recomp_dbgui_pop_id();
        }

        recomp_dbgui_end_child();
    }
}

static void priority_list_tab(Object** objects, s32 count) {
    recomp_dbgui_text("Object Priority/Update List:");
    if (recomp_dbgui_begin_child("object_priority_list")) {
        s32 i = 0;
        void *node = gObjUpdateList.head;

        while (node != NULL) {
            Object *obj = (Object*)node;
            
            recomp_dbgui_push_str_id(recomp_sprintf_helper("%p", obj));

            if (recomp_dbgui_button("Edit")) {
                add_edit_object(obj);
            }

            recomp_dbgui_same_line();

            recomp_dbgui_textf("[%d] [%d] %s", i, obj->updatePriority, obj->def->name);

            recomp_dbgui_pop_id();

            node = *LINKED_LIST_NEXT_FIELD3(gObjUpdateList, node);
            i++;
        }

        recomp_dbgui_end_child();
    }
}

RECOMP_CALLBACK(".", my_debug_menu_event) void object_debug_menu_callback() {
    recomp_dbgui_menu_item("Objects", &object_debug_window_open);
}

RECOMP_CALLBACK(".", my_dbgui_event) void object_debug_dbgui_callback() {
    s32 hovered_object_idx = -1;

    s32 _, count;
    Object** objects = get_world_objects(&_, &count);

    if (object_debug_window_open) {
        if (recomp_dbgui_begin("Object Debug", &object_debug_window_open)) {
            if (recomp_dbgui_begin_tab_bar("##objects")) {
                if (recomp_dbgui_begin_tab_item("Objects", NULL)) {
                    objects_list_tab(objects, count, &hovered_object_idx);
                    recomp_dbgui_end_tab_item();
                }

                if (recomp_dbgui_begin_tab_item("Priority List", NULL)) {
                    priority_list_tab(objects, count);
                    recomp_dbgui_end_tab_item();
                }
                
                recomp_dbgui_end_tab_bar();
            }
        }
        recomp_dbgui_end();
    }

    for (s32 i = 0; i < (s32)(sizeof(edit_objects) / sizeof(Object*)); i++) {
        if (edit_objects[i] != NULL) {
            object_editor(edit_objects[i], i);
        }
    }

    if (show_in_world) {
        for (s32 i = 0; i < count; i++) {
            s32 hovered = hovered_object_idx == i;

            Object *obj = objects[i];

            // position 3 is the absolute world pos including parent positions
            Vec3f position = obj->positionMirror3;

            SRT srt = obj->srt;
            srt.scale = 1.0f;

            MtxF mtx;
            matrix_from_srt(&mtx, &srt);

            if (obj->parent != NULL) {
                SRT psrt = obj->parent->srt;
                psrt.scale = 1.0f;

                MtxF pmtx;
                matrix_from_srt(&pmtx, &psrt);

                matrix_concat(&mtx, &pmtx, &mtx);
            }

            draw_3d_text(
                position.x,
                position.y,
                position.z,
                recomp_sprintf_helper("[%d] %s", i, obj->def->name),
                hovered ? 0xFFFFFFFF : 0xFF00FFFF
            );

            if (show_hitboxes) {
                if (obj->objhitInfo != NULL) {
                    s16 v1 = *(s16*)((u32)obj->objhitInfo + 0x52);
                    s16 v2 = *(s16*)((u32)obj->objhitInfo + 0x54);
                    s16 v3 = *(s16*)((u32)obj->objhitInfo + 0x56);
                    s16 v4 = *(s16*)((u32)obj->objhitInfo + 0x58);

                    draw_3d_box(
                        &mtx, 
                        v1 * 2,
                        v3,
                        v1 * 2,
                        hovered ? 0xFFFFFFFF : 0xFFFF00FF
                    );
                } else {
                    draw_3d_sphere(
                        position.x,
                        position.y,
                        position.z,
                        2 * obj->srt.scale,
                        hovered ? 0xFFFFFFFF : 0xFFFF00FF
                    );
                }
            }
        }
    }

    remove_freed_edit_objects();
}