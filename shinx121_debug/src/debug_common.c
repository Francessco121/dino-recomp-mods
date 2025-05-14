#include "debug_common.h"

#include "dbgui.h"

#include "PR/ultratypes.h"

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
