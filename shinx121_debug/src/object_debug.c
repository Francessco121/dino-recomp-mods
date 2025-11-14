#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "3d.h"
#include "debug_common.h"
#include "object_editor.h"
#include "object_dll_ids.h"
#include "objects/bwlog_debug.h"
#include "objects/dll27_debug.h"
#include "objects/kt_rex_debug.h"
#include "objects/object_debug.h"
#include "objects/objfsa_debug.h"
#include "objects/seqobj_debug.h"
#include "objects/snowbike_debug.h"
#include "objects/trigger_debug.h"

#include "libc/string.h"
#include "dlls/engine/18_objfsa.h"
#include "dlls/engine/27.h"
#include "game/objects/object.h"
#include "game/objects/object_id.h"
#include "sys/objects.h"
#include "sys/objtype.h"
#include "sys/linked_list.h"

#define OBJECT_TYPE_LIST_LENGTH 256
#define OBJECT_MAX_TYPES 65

extern s16 gObjectTypeIndices[OBJECT_MAX_TYPES + 1];
extern s16 gObjectTypeListCount;
extern Object *gObjectTypeList[OBJECT_TYPE_LIST_LENGTH];

extern LinkedList gObjUpdateList;

static s32 object_debug_window_open = FALSE;
static s32 hovered_object_idx = -1;
static s32 show_in_world = FALSE;
static s32 show_hitboxes = TRUE;
static s32 show_modlines = FALSE;
static char search_buffer[256] = {0};
static s32 type_filter = 0;
static s32 filter_types = FALSE;
static Object *edit_objects[256] = {NULL};
static ObjEditorData edit_objects_data[256] = {NULL};

static void add_edit_object(Object *obj) {
    for (s32 i = 0; i < (s32)(sizeof(edit_objects) / sizeof(Object*)); i++) {
        if (edit_objects[i] == obj) {
            return;
        }
    }

    for (s32 i = 0; i < (s32)(sizeof(edit_objects) / sizeof(Object*)); i++) {
        if (edit_objects[i] == NULL) {
            edit_objects[i] = obj;
            bzero(&edit_objects_data[i], sizeof(edit_objects_data[i]));
            edit_objects_data[i].seqActorBits = -1;
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

static void object_editor(Object *obj, ObjEditorData *editorData, s32 index) {
    s32 open = TRUE;
    s32 destroy = FALSE;
    if (dbgui_begin(recomp_sprintf_helper("%s###object_edit_%p", obj->def->name, obj), &open)) {
        if (dbgui_begin_tab_bar(recomp_sprintf_helper("%s###object_edit_%p_tabbar", obj->def->name, obj))) {
            // Main tab
            if (dbgui_begin_tab_item("Object", NULL)) {
                if (dbgui_button("Destroy")) {
                    destroy = TRUE;
                }
                object_edit_contents(obj);
                dbgui_end_tab_item();
            }
            // Common tabs
            if (obj->def->pSeq != NULL) {
                if (dbgui_begin_tab_item("Sequences", NULL)) {
                    object_seq_debug(obj, editorData);
                    dbgui_end_tab_item();
                }
            }
            // DLL 27 tab
            DLL27_Data *dll27Data = dll27_debug_get_data(obj);
            if (dll27Data != NULL) {
                if (dbgui_begin_tab_item("DLL 27 Data", NULL)) {
                    dll27_debug_tab(obj, dll27Data);
                    dbgui_end_tab_item();
                }
            }
            // Objfsa tab
            ObjFSA_Data *fsa = objfsa_debug_get_data(obj);
            if (fsa != NULL) {
                if (dbgui_begin_tab_item("Obj FSA Data", NULL)) {
                    objfsa_debug_tab(obj, fsa);
                    dbgui_end_tab_item();
                }
            }
            // DLL specific tabs
            switch (obj->def->dllID) {
                case DLL_ID_SEQOBJ:
                    if (dbgui_begin_tab_item("SEQOBJ", NULL)) {
                        seqobj_debug_tab(obj);
                        dbgui_end_tab_item();
                    }
                    break;
                case DLL_ID_trigger:
                    if (dbgui_begin_tab_item("Trigger", NULL)) {
                        trigger_debug_tab(obj);
                        dbgui_end_tab_item();
                    }
                    break;
                case DLL_ID_KT_Rex:
                    if (dbgui_begin_tab_item("KT_Rex", NULL)) {
                        kt_rex_debug_tab(obj);
                        dbgui_end_tab_item();
                    }
                    break;
                case DLL_ID_IMSnowBike:
                    if (dbgui_begin_tab_item("IMSnowBike", NULL)) {
                        snowbike_debug_tab(obj);
                        dbgui_end_tab_item();
                    }
                    break;
                case DLL_ID_BWLog:
                    if (dbgui_begin_tab_item("BWLog", NULL)) {
                        bwlog_debug_tab(obj);
                        dbgui_end_tab_item();
                    }
                    break;
            }

            dbgui_end_tab_bar();
        }
    }
    dbgui_end();

    if (destroy) {
        obj_destroy_object(obj);
        open = FALSE;
    }

    if (!open) {
        edit_objects[index] = NULL;
    }
}

static void objects_list_tab(Object** objects, s32 count, s32 *hovered_object_idx) {
    dbgui_checkbox("Show in world", &show_in_world);

    if (show_in_world) {
        dbgui_checkbox("Show 3D boxes", &show_hitboxes);
        dbgui_checkbox("Show 3D mod lines", &show_modlines);
    }

    dbgui_checkbox("Filter by Type", &filter_types);
    if (filter_types) {
        dbgui_same_line();
        dbgui_set_next_item_width(100);
        dbgui_input_int("Type", &type_filter);
    }

    dbgui_input_text("Search", search_buffer, sizeof(search_buffer) / sizeof(char));
    s32 search_len = strlen(search_buffer);

    dbgui_textf("Objects (%d):", count);
    if (dbgui_begin_child("object_list")) {
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

            dbgui_push_str_id(recomp_sprintf_helper("%p", obj));

            if (dbgui_button("Edit")) {
                add_edit_object(obj);
            }

            dbgui_same_line();

            dbgui_textf("[%d] %s", i, name);

            if (dbgui_is_item_hovered()) {
                *hovered_object_idx = i;
            }

            dbgui_pop_id();
        }

        dbgui_end_child();
    }
}

static void priority_list_tab() {
    dbgui_text("Object Priority/Update List:");
    if (dbgui_begin_child("object_priority_list")) {
        s32 i = 0;
        void *node = gObjUpdateList.head;

        while (node != NULL) {
            Object *obj = (Object*)node;
            
            dbgui_push_str_id(recomp_sprintf_helper("%p", obj));

            if (dbgui_button("Edit")) {
                add_edit_object(obj);
            }

            dbgui_same_line();

            dbgui_textf("[%d] [%d] %s", i, obj->updatePriority, obj->def->name);

            dbgui_pop_id();

            node = *LINKED_LIST_NEXT_FIELD3(gObjUpdateList, node);
            i++;
        }

        dbgui_end_child();
    }
}

static void type_list_tab() {
    dbgui_text("Object Type List:");
    if (dbgui_begin_child("object_type_list")) {
        s32 first = TRUE;

        for (s32 type = 0; type < 64; type++) {
            s32 listStart = gObjectTypeIndices[type];
            s32 listEnd = gObjectTypeIndices[type + 1];
            s32 count = listEnd - listStart;

            if (count <= 0) {
                continue;
            }

            if (!first) {
                dbgui_separator();
            } else {
                first = FALSE;
            }
            
            dbgui_textf("Type %d (%d):", type, count);

            for (s32 k = 0; k < count; k++) {
                Object *obj = gObjectTypeList[listStart + k];
                if (obj == NULL) {
                    dbgui_textf("[%d] (null)", k);
                    continue;
                }

                dbgui_push_str_id(recomp_sprintf_helper("%p", obj));

                if (dbgui_button("Edit")) {
                    add_edit_object(obj);
                }

                dbgui_same_line();

                dbgui_textf("[%d] %s", k, obj->def->name);

                dbgui_pop_id();
            }
        }

        dbgui_end_child();
    }
}

RECOMP_CALLBACK(".", my_debug_menu_event) void object_debug_menu_callback() {
    dbgui_menu_item("Objects", &object_debug_window_open);
}

RECOMP_CALLBACK(".", my_dbgui_event) void object_debug_dbgui_callback() {
    s32 hovered_object_idx = -1;

    s32 _, count;
    Object** objects;
    if (filter_types) {
        objects = obj_get_all_of_type(type_filter, &count);
    } else {
        objects = get_world_objects(&_, &count);
    }

    if (object_debug_window_open) {
        if (dbgui_begin("Object Debug", &object_debug_window_open)) {
            if (dbgui_begin_tab_bar("##objects")) {
                if (dbgui_begin_tab_item("Objects", NULL)) {
                    objects_list_tab(objects, count, &hovered_object_idx);
                    dbgui_end_tab_item();
                }

                if (dbgui_begin_tab_item("Priority List", NULL)) {
                    priority_list_tab();
                    dbgui_end_tab_item();
                }

                if (dbgui_begin_tab_item("Type List", NULL)) {
                    type_list_tab();
                    dbgui_end_tab_item();
                }
                
                dbgui_end_tab_bar();
            }
        }
        dbgui_end();
    }

    for (s32 i = 0; i < (s32)(sizeof(edit_objects) / sizeof(Object*)); i++) {
        Object *obj = edit_objects[i];
        
        if (obj != NULL) {
            s32 stillExists = FALSE;
            for (s32 k = 0; k < count; k++) {
                if (objects[k] == obj) {
                    stillExists = TRUE;
                    break;
                }
            }

            if (stillExists) {
                object_editor(obj, &edit_objects_data[i], i);
            } else {
                edit_objects[i] = NULL;
            }
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
                switch (obj->def->dllID) {
                case DLL_ID_trigger:
                    draw_trigger(obj, hovered ? 0xFFFFFFFF : 0xFFFF00FF);
                    break;
                default:
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
                    break;
                }
            }

            if (show_modlines) {
                if (obj->def->pModLines != NULL) {
                    for (s32 k = 0; k < obj->def->modLineCount; k++) {
                        HitsLineCustom *hitLine = (HitsLineCustom*)&obj->def->pModLines[k];

                        f32 centerX = (f32)(hitLine->Ax + hitLine->Bx) / 2;
                        f32 centerY = (f32)(hitLine->Ay + hitLine->By) / 2;
                        f32 centerZ = (f32)(hitLine->Az + hitLine->Bz) / 2;

                        s32 heightA, heightB;
                        if (hitLine->settingsA & 0x80) {
                            heightA = hitLine->height;
                            heightB = hitLine->height;
                        } else {
                            heightA = hitLine->heightA;
                            heightB = hitLine->heightB;
                        }

                        f32 tx, ty, tz;

                        vec3_transform(&mtx, centerX, centerY, centerZ, &tx, &ty, &tz);
                        draw_3d_text(tx, ty, tz, 
                            recomp_sprintf_helper("ObjModLine %d/%d", i, k), 
                            0xFF00FFFF);

                        f32 tx1, ty1, tz1;
                        f32 tx2, ty2, tz2;
                        
                        vec3_transform(&mtx, hitLine->Ax, hitLine->Ay, hitLine->Az, &tx1, &ty1, &tz1);
                        vec3_transform(&mtx, hitLine->Bx, hitLine->By, hitLine->Bz, &tx2, &ty2, &tz2);
                        draw_3d_line(
                            tx1, ty1, tz1, 
                            tx2, ty2, tz2, 
                            0xFFFF00FF);

                        vec3_transform(&mtx, hitLine->Ax, hitLine->Ay, hitLine->Az, &tx1, &ty1, &tz1);
                        vec3_transform(&mtx, hitLine->Ax, hitLine->Ay + heightA, hitLine->Az, &tx2, &ty2, &tz2);
                        draw_3d_line(
                            tx1, ty1, tz1, 
                            tx2, ty2, tz2, 
                            0x888800CC);

                        vec3_transform(&mtx, hitLine->Bx, hitLine->By, hitLine->Bz, &tx1, &ty1, &tz1);
                        vec3_transform(&mtx, hitLine->Bx, hitLine->By + heightB, hitLine->Bz, &tx2, &ty2, &tz2);
                        draw_3d_line(
                            tx1, ty1, tz1, 
                            tx2, ty2, tz2, 
                            0x888800CC);

                        vec3_transform(&mtx, hitLine->Ax, hitLine->Ay + heightA, hitLine->Az, &tx1, &ty1, &tz1);
                        vec3_transform(&mtx, hitLine->Bx, hitLine->By + heightB, hitLine->Bz, &tx2, &ty2, &tz2);
                        draw_3d_line(
                            tx1, ty1, tz1, 
                            tx2, ty2, tz2, 
                            0x888800CC);
                    }
                }
            }
        }
    }

    remove_freed_edit_objects();
}