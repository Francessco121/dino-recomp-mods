#include "debug_common.h"

#include "dbgui.h"

#include "PR/ultratypes.h"
#include "game/objects/object.h"
#include "sys/math.h"

void dbgui_input_byte(const char *label, u8 *value) {
    s32 value_ = *value;
    if (dbgui_input_int(label, &value_)) {
        *value = (u8)value_;
    }
}

s32 dbgui_input_byte_ext(const char *label, u8 *value, const DbgUiInputIntOptions *options) {
    s32 value_ = *value;
    if (dbgui_input_int_ext(label, &value_, options)) {
        *value = (u8)value_;
        return TRUE;
    }

    return FALSE;
}

s32 dbgui_input_sbyte(const char *label, s8 *value) {
    s32 value_ = *value;
    if (dbgui_input_int(label, &value_)) {
        *value = (s8)value_;
        return TRUE;
    }

    return FALSE;
}

s32 dbgui_input_sbyte_ext(const char *label, s8 *value, const DbgUiInputIntOptions *options) {
    s32 value_ = *value;
    if (dbgui_input_int_ext(label, &value_, options)) {
        *value = (s8)value_;
        return TRUE;
    }

    return FALSE;
}

void dbgui_input_short(const char *label, s16 *value) {
    s32 value_ = *value;
    if (dbgui_input_int(label, &value_)) {
        *value = (s16)value_;
    }
}

void dbgui_input_short_ext(const char *label, s16 *value, const DbgUiInputIntOptions *options) {
    s32 value_ = *value;
    if (dbgui_input_int_ext(label, &value_, options)) {
        *value = (s16)value_;
    }
}

void dbgui_input_uint(const char *label, u32 *value) {
    s32 value_ = *value;
    if (dbgui_input_int(label, &value_)) {
        *value = (u32)value_;
    }
}

void dbgui_input_vec3f(const char *label, Vec3f *value) {
    Vec3f value_ = *value;
    s32 changed = FALSE;
    dbgui_push_str_id(label);

    dbgui_text(label);
    dbgui_same_line();
    dbgui_set_next_item_width(80);
    if (dbgui_input_float("x", &value_.x)) {
        changed = TRUE;
    }
    dbgui_same_line();
    dbgui_set_next_item_width(80);
    if (dbgui_input_float("y", &value_.y)) {
        changed = TRUE;
    }
    dbgui_same_line();
    dbgui_set_next_item_width(80);
    if (dbgui_input_float("z", &value_.z)) {
        changed = TRUE;
    }

    if (changed) {
        *value = value_;
    }

    dbgui_pop_id();
}

void dbgui_input_obj_position(const char *label, Object *value) {
    Vec3f value_ = value->srt.transl;

    s32 changed = FALSE;
    dbgui_push_str_id(label);

    dbgui_text(label);
    dbgui_same_line();
    dbgui_set_next_item_width(80);
    if (dbgui_input_float("x", &value_.x)) {
        changed = TRUE;
    }
    dbgui_same_line();
    dbgui_set_next_item_width(80);
    if (dbgui_input_float("y", &value_.y)) {
        changed = TRUE;
    }
    dbgui_same_line();
    dbgui_set_next_item_width(80);
    if (dbgui_input_float("z", &value_.z)) {
        changed = TRUE;
    }

    if (changed) {
        value->srt.transl = value_;
        value->positionMirror = value_;
        value->positionMirror2 = value_;
        value->positionMirror3 = value_;
    }

    dbgui_pop_id();
}
