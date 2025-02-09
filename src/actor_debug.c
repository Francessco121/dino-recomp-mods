#include "modding.h"
#include "imports.h"
#include "common.h"

#include "3d.h"

extern TActor** get_world_actors(s32*, s32 *count);

static s32 actor_debug_window_open = FALSE;
static s32 hovered_actor_idx = -1;
static s32 show_in_world = FALSE;
static s32 show_hitboxes = TRUE;
static char search_buffer[256] = {0};
static TActor *edit_actors[256] = {NULL};

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

static void add_edit_actor(TActor *actor) {
    for (s32 i = 0; i < (s32)(sizeof(edit_actors) / sizeof(TActor*)); i++) {
        if (edit_actors[i] == actor) {
            return;
        }
    }

    for (s32 i = 0; i < (s32)(sizeof(edit_actors) / sizeof(TActor*)); i++) {
        if (edit_actors[i] == NULL) {
            edit_actors[i] = actor;
            break;
        }
    }
}

static void remove_freed_edit_actors() {
    s32 _, count;
    TActor** actors = get_world_actors(&_, &count);

    for (s32 i = 0; i < (s32)(sizeof(edit_actors) / sizeof(TActor*)); i++) {
        TActor *actor = edit_actors[i];
        if (edit_actors[i] != NULL) {
            s32 found = FALSE;
            for (s32 k = 0; k < count; k++) {
                if (actors[k] == actor) {
                    found = TRUE;
                    break;
                }
            }

            if (!found) {
                edit_actors[i] = NULL;
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

static void dbgui_input_sbyte(const char *label, s8 *value) {
    s32 value_ = *value;
    if (recomp_dbgui_input_int(label, &value_)) {
        *value = (s8)value_;
    }
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

static void actor_edit_contents(TActor *actor) {
    recomp_dbgui_textf("Position: %d,%d,%d", 
        (s32)actor->srt.transl.x, (s32)actor->srt.transl.y, (s32)actor->srt.transl.z);

    recomp_dbgui_textf("Rotation (y,p,r): %d,%d,%d", 
        actor->srt.yaw, actor->srt.pitch, actor->srt.roll);
    
    recomp_dbgui_textf("Scale: %f", actor->srt.scale);

    dbgui_input_short("Flags", &actor->srt.flags);

    recomp_dbgui_textf("Speed: %d,%d,%d", 
        (s32)actor->speed.x, (s32)actor->speed.y, (s32)actor->speed.z);

    if (actor->linkedActor != NULL) {
        if (recomp_dbgui_tree_node("linkedActor")) {
            actor_edit_contents(actor->linkedActor);
            recomp_dbgui_tree_pop();
        }
    }
    
    dbgui_input_byte("unk_0x34", &actor->unk_0x34);
    dbgui_input_sbyte("matrixIdx", &actor->matrixIdx);
    dbgui_input_byte("unk_0x36", &actor->unk_0x36);
    dbgui_input_byte("unk_0x37", &actor->unk_0x37);
    dbgui_input_uint("unk_0x38", &actor->unk_0x38);
    recomp_dbgui_textf("objId: %u", actor->objId);
    recomp_dbgui_textf("unk0x46: %d", actor->unk0x46);
    recomp_dbgui_textf("ptr0x48: %p", actor->ptr0x48);
    if (actor->unk0x4c != NULL && recomp_dbgui_tree_node("unk0x4c")) {
        ActorUnk0x1a *unk0x4c = actor->unk0x4c;
        recomp_dbgui_textf("unk0x1a: %d", unk0x4c->unk0x1a);
        recomp_dbgui_tree_pop();
    }
    if (actor->data != NULL && recomp_dbgui_tree_node("data")) {
        ObjData *data = actor->data;
        recomp_dbgui_textf("dll: %d", resolve_dll_number(*(u16*)((u32)data + 0x58)));
        recomp_dbgui_textf("modLinesIdx: %d", data->modLinesIdx); // TODO: this is model inst count?
        recomp_dbgui_textf("name: %s", (char*)((u32)data + 0x5f));
        recomp_dbgui_tree_pop();
    }
    if (actor->objhitInfo != NULL && recomp_dbgui_tree_node("objhitInfo")) {
        ActorObjhitInfo *objhitInfo = actor->objhitInfo;
        recomp_dbgui_textf("unk_0x10: %d,%d,%d", 
            (s32)objhitInfo->unk_0x10.x, (s32)objhitInfo->unk_0x10.y, (s32)objhitInfo->unk_0x10.z);
        recomp_dbgui_textf("unk_0x20: %d,%d,%d", 
            (s32)objhitInfo->unk_0x20.x, (s32)objhitInfo->unk_0x20.y, (s32)objhitInfo->unk_0x20.z);
        recomp_dbgui_textf("unk_0x5a: %d", objhitInfo->unk_0x5a);
        recomp_dbgui_textf("unk_0x9f: %d", objhitInfo->unk_0x9f);
        recomp_dbgui_tree_pop();
    }
    recomp_dbgui_textf("unk0x58: %d", actor->unk0x58);
    recomp_dbgui_textf("ptr0x5c: %p", actor->ptr0x5c);
    recomp_dbgui_textf("ptr0x60: %p", actor->ptr0x60);

    recomp_dbgui_textf("dll: %p", actor->dll);
    recomp_dbgui_textf("ptr0x6c: %p", actor->ptr0x6c);
    recomp_dbgui_textf("ptr0x70: %p", actor->ptr0x70);
    recomp_dbgui_textf("unk0x74: %d", actor->unk0x74);
    recomp_dbgui_textf("unk_0x78: %d", actor->unk_0x78);
    recomp_dbgui_textf("positionMirror2: %d,%d,%d", 
        (s32)actor->positionMirror2.x, (s32)actor->positionMirror2.y, (s32)actor->positionMirror2.z);
    recomp_dbgui_textf("positionMirror3: %d,%d,%d", 
        (s32)actor->positionMirror3.x, (s32)actor->positionMirror3.y, (s32)actor->positionMirror3.z);
    recomp_dbgui_textf("animTimer: %d", (s32)actor->animTimer);
    recomp_dbgui_textf("unk0x9c: %d", (s32)actor->unk0x9c);
    recomp_dbgui_textf("curAnimId: %d", actor->curAnimId);
    recomp_dbgui_textf("unk_0xa2: %d", actor->unk_0xa2);
    dbgui_input_sbyte("modelInstIdx", &actor->modelInstIdx);
    recomp_dbgui_textf("unk0xae: %d", actor->unk0xae);
    recomp_dbgui_textf("unk0xaf: %d", actor->unk0xaf);
    recomp_dbgui_textf("unk0xb0: %u", actor->unk0xb0);
    recomp_dbgui_textf("state: %p", actor->state);
    recomp_dbgui_textf("unk0xbc: %p", actor->unk0xbc);
    recomp_dbgui_textf("unk0xc0: %p", actor->unk0xc0);
    recomp_dbgui_textf("unk0xc4: %u", actor->unk0xc4);
    if (actor->linkedActor2 != NULL) {
        if (recomp_dbgui_tree_node("linkedActor2")) {
            actor_edit_contents(actor->linkedActor2);
            recomp_dbgui_tree_pop();
        }
    }
    recomp_dbgui_textf("ptr0xcc: %p", actor->ptr0xcc);
    recomp_dbgui_textf("unk0xdc: %d", actor->unk0xdc);
}

static void actor_editor(TActor *actor, s32 index) {
    char *name = (char*)((u32)actor->data + 0x5f);

    s32 open = TRUE;
    if (recomp_dbgui_begin(recomp_sprintf_helper("%s###actor_edit_%p", name, actor), &open)) {
        actor_edit_contents(actor);
    }
    recomp_dbgui_end();

    if (!open) {
        edit_actors[index] = NULL;
    }
}

RECOMP_CALLBACK(".", my_debug_menu_event) void actor_debug_menu_callback() {
    recomp_dbgui_menu_item("Actors", &actor_debug_window_open);
}

RECOMP_CALLBACK(".", my_dbgui_event) void actor_debug_dbgui_callback() {
    s32 hovered_actor_idx = -1;

    s32 _, count;
    TActor** actors = get_world_actors(&_, &count);

    if (actor_debug_window_open) {
        if (recomp_dbgui_begin("Actors", &actor_debug_window_open)) {
            recomp_dbgui_checkbox("Show in world", &show_in_world);

            if (show_in_world) {
                recomp_dbgui_checkbox("Show hitboxes", &show_hitboxes);
            }

            recomp_dbgui_input_text("Search", search_buffer, sizeof(search_buffer) / sizeof(char));
            s32 search_len = strlen(search_buffer);

            recomp_dbgui_textf("Actors (%d):", count);
            if (recomp_dbgui_begin_child("actor_list")) {
                for (s32 i = 0; i < count; i++) {
                    TActor *actor = actors[i];
                    char *name = (char*)((u32)actor->data + 0x5f);

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

                    recomp_dbgui_push_str_id(recomp_sprintf_helper("%p", actor));

                    if (recomp_dbgui_button("Edit")) {
                        add_edit_actor(actor);
                    }

                    recomp_dbgui_same_line();

                    recomp_dbgui_textf("[%d] %s", i, name);

                    if (recomp_dbgui_is_item_hovered()) {
                        hovered_actor_idx = i;
                    }

                    recomp_dbgui_pop_id();
                }

                recomp_dbgui_end_child();
            }
        }
        recomp_dbgui_end();
    }

    for (s32 i = 0; i < (s32)(sizeof(edit_actors) / sizeof(TActor*)); i++) {
        if (edit_actors[i] != NULL) {
            actor_editor(edit_actors[i], i);
        }
    }

    if (show_in_world) {
        for (s32 i = 0; i < count; i++) {
            s32 hovered = hovered_actor_idx == i;

            TActor *actor = actors[i];
            char *name = (char*)((u32)actor->data + 0x5f);

            draw_3d_text(
                actor->srt.transl.x,
                actor->srt.transl.y,
                actor->srt.transl.z,
                recomp_sprintf_helper("[%d] %s", i, name),
                hovered ? 0xFFFFFFFF : 0xFF00FFFF
            );

            if (show_hitboxes) {
                if (actor->objhitInfo != NULL) {
                    s16 v1 = *(s16*)((u32)actor->objhitInfo + 0x52);
                    s16 v2 = *(s16*)((u32)actor->objhitInfo + 0x54);
                    s16 v3 = *(s16*)((u32)actor->objhitInfo + 0x56);
                    s16 v4 = *(s16*)((u32)actor->objhitInfo + 0x58);

                    draw_3d_box(
                        actor->srt.transl.x, 
                        actor->srt.transl.y + (v3 / 2), 
                        actor->srt.transl.z,  
                        v1 * 2,
                        v3,
                        v1 * 2,
                        hovered ? 0xFFFFFFFF : 0xFFFF00FF
                    );
                } else {
                    draw_3d_sphere(
                        actor->srt.transl.x,
                        actor->srt.transl.y,
                        actor->srt.transl.z,
                        2 * actor->srt.scale,
                        hovered ? 0xFFFFFFFF : 0xFFFF00FF
                    );
                }
            }
        }
    }

    remove_freed_edit_actors();
}