#include "modding.h"
#include "dbgui.h"
#include "common.h"
#include "recomputils.h"
#include "sys/fs.h"
#include "dll.h"

typedef struct MusicAction {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u8 unk10; // amseq channel? 0-3?
    u8 unk11; // music/ambient ID
    u8 unk12;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    s16 unk16;
    s16 unk18;
    u16 unk1A;
    u32 unk1C;
} MusicAction;

static s32 recomp_play_music_action(MusicAction *action, s8 arg1, s32 arg2);

static const DbgUiInputIntOptions hexInput = {
    .step = 1,
    .stepFast = 0x10,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

extern s32 *gFile_MPEG_TAB;
extern void mpeg_fs_play(s32 id);

static s32 sound_test_window_open = FALSE;
static s32 sfxID;
static s32 lastSfxID = -1;
static s32 musicAction;
static s32 lastMusicAction = -1;
static s32 volume = 127;
static s32 mpegID;
static s32 numMPEGEntries = -1;

RECOMP_CALLBACK(".", my_debug_menu_event) void sound_test_menu_callback() {
    dbgui_menu_item("Sound Test", &sound_test_window_open);
}

RECOMP_CALLBACK(".", my_dbgui_event) void sound_test_dbgui_callback() {
    if (numMPEGEntries < 0) {
        numMPEGEntries = (get_file_size(MPEG_TAB) / 4) - 1;
    }

    if (sound_test_window_open) {
        if (dbgui_begin("Sound Test", &sound_test_window_open)) {
            if (dbgui_begin_tab_bar("sound_test_tab_bar")) {
                if (dbgui_begin_tab_item("AMSFX", NULL)) {
                    dbgui_set_next_item_width(120);
                    if (dbgui_input_int_ext("ID", &sfxID, &hexInput)) {
                        if (sfxID < 0) sfxID = 0;
                    }
                    dbgui_same_line();
                    dbgui_set_next_item_width(100);
                    if (dbgui_input_int("Volume", &volume)) {
                        if (volume < 0) volume = 0;
                        if (volume > 255) volume = 255;
                    }

                    if (dbgui_button("Play")) {
                        if (lastSfxID != -1) {
                            gDLL_6_AMSFX->vtbl->func_A1C(lastSfxID);
                        }
                        lastSfxID = gDLL_6_AMSFX->vtbl->play_sound(NULL, sfxID, volume, NULL, 0, 0, 0);
                    }
                    dbgui_same_line();
                    if (dbgui_button("Stop")) {
                        if (lastSfxID != -1) {
                            gDLL_6_AMSFX->vtbl->func_A1C(lastSfxID);
                        }
                    }

                    dbgui_end_tab_item();
                }

                if (dbgui_begin_tab_item("AMSEQ (Music)", NULL)) {
                    static MusicAction killAction = { .unk11 = 0 };

                    if (dbgui_button("Stop Ambience 1")) {
                        killAction.unk10 = 0;
                        recomp_play_music_action(&killAction, 0, 0);
                    }
                    dbgui_same_line();
                    if (dbgui_button("Stop Ambience 2")) {
                        killAction.unk10 = 1;
                        recomp_play_music_action(&killAction, 0, 0);
                    }

                    if (dbgui_button("Stop Music 1")) {
                        killAction.unk10 = 2;
                        recomp_play_music_action(&killAction, 0, 0);
                    }
                    dbgui_same_line();
                    if (dbgui_button("Stop Music 2")) {
                        killAction.unk10 = 3;
                        recomp_play_music_action(&killAction, 0, 0);
                    }

                    dbgui_separator();

                    if (dbgui_begin_child("scroll")) {
                        dbgui_set_next_item_width(100);
                        if (dbgui_input_int("Music Action ID", &musicAction)) {
                            if (musicAction < 0) musicAction = 0;
                        }

                        if (dbgui_button("Play")) {
                            if (lastMusicAction != -1 && gDLL_5_AMSEQ->vtbl->func2(NULL, lastMusicAction)) {
                                gDLL_5_AMSEQ->vtbl->func1(NULL, lastMusicAction, 0, 0, 0);
                            }
                            gDLL_5_AMSEQ->vtbl->func0(NULL, musicAction, 0, 0, 0);
                            lastMusicAction = musicAction;
                        }

                        dbgui_text("Note: Music actions also control ambience.");

                        dbgui_separator();

                        dbgui_text("Custom Music Action:");
                        dbgui_push_str_id("custom_music_action");

                        static MusicAction musicAction = { 
                            .unk4 = 655440, 
                            .unk10 = 2, 
                            .unk11 = 0, 
                            .unk12 = 127, 
                            .unk13 = 60, 
                            .unk15 = 20, 
                            .unk16 = -1, 
                            .unk18 = -1
                        };

                        s32 audioID = musicAction.unk11;
                        if (dbgui_input_int("ID", &audioID)) {
                            if (audioID < 0) audioID = 0;
                            musicAction.unk11 = audioID;
                        }
                        s32 channel = musicAction.unk10;
                        static const char *channelNames[] = {"Ambience 1", "Ambience 2", "Music 1", "Music 2"};
                        if (dbgui_begin_combo("Channel", channelNames[channel])) {
                            for (s32 i = 0; i < 4; i++) {
                                if (dbgui_selectable(channelNames[i], channel == i)) {
                                    musicAction.unk10 = i;
                                }
                            }

                            dbgui_end_combo();
                        }
                        s32 unk12 = musicAction.unk12;
                        if (dbgui_input_int("Volume", &unk12)) {
                            if (unk12 < 0) unk12 = 0;
                            if (unk12 > 127) unk12 = 127;
                            musicAction.unk12 = unk12;
                        }
                        s32 unk16 = musicAction.unk16;
                        if (dbgui_tree_node("Tracks (Instruments)")) {
                            if (dbgui_button("Reset")) {
                                musicAction.unk16 = -1;
                            }
                            for (s32 k = 0; k < 16; k++) {
                                s32 status = (1 << k) & unk16;
                                if (dbgui_checkbox(recomp_sprintf_helper("%d", k), &status)) {
                                    if (status) {
                                        unk16 |= (1 << k);
                                    } else {
                                        unk16 &= ~(1 << k);
                                    }
                                    musicAction.unk16 = unk16;
                                }
                                if (((k + 1) % 4) != 0) {
                                    dbgui_same_line();
                                }
                            }
                            
                            dbgui_tree_pop();
                        }

                        if (dbgui_tree_node("Advanced")) {
                            s32 unk13 = musicAction.unk13;
                            if (dbgui_input_int("unk13", &unk13)) {
                                musicAction.unk13 = unk13;
                            }
                            s32 unk15 = musicAction.unk15;
                            if (dbgui_input_int("unk15", &unk15)) {
                                musicAction.unk15 = unk15;
                            }
                            s32 unk18 = musicAction.unk18;
                            if (dbgui_input_int("unk18", &unk18)) {
                                musicAction.unk18 = unk18;
                            }
                            
                            dbgui_tree_pop();
                        }

                        if (musicAction.unk10 < 2) { // channel
                            if (musicAction.unk11 > 38) musicAction.unk11 = 38;
                        } else {
                            if (musicAction.unk11 > 110) musicAction.unk11 = 110;
                        }

                        if (musicAction.unk10 < 2 && musicAction.unk11 == 3) {
                            dbgui_text("WARNING: Ambience ID 3 will crash partially\ninto playback!");
                        }

                        if (dbgui_button("Play")) {
                            recomp_play_music_action(&musicAction, 0, 0);
                        }

                        dbgui_pop_id();

                        dbgui_end_child();
                    }

                    dbgui_end_tab_item();
                }

                if (dbgui_begin_tab_item("MPEG", NULL)) {
                    if (dbgui_input_int("ID", &mpegID)) {
                        if (mpegID < 0) mpegID = 0;
                        if (mpegID >= numMPEGEntries) mpegID = numMPEGEntries - 1;
                    }

                    if (dbgui_button("Play")) {
                        mpeg_fs_play(mpegID);
                    }

                    dbgui_end_tab_item();
                }
                
                dbgui_end_tab_bar();
            }
        }
        dbgui_end();
    }
}

#include "recomp/dlls/engine/5_AMSEQ_recomp.h"

extern s32 dll_5_func_1E8C(MusicAction *action, s8 arg1, s32 arg2);

static s32 recomp_play_music_action(MusicAction *action, s8 arg1, s32 arg2) {
    return dll_5_func_1E8C(action, arg1, arg2);
}
