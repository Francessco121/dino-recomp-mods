#include "modding.h"
#include "recomputils.h"
#include "dbgui.h"

#include "common.h"
#include "sys/asset_thread.h"
#include "sys/fs.h"
#include "dlls/engine/5_amseq.h"
#include "dll.h"

static s32 recomp_play_music_action(MusicAction *action, s8 arg1, s32 arg2);

static const DbgUiInputIntOptions hexInput = {
    .step = 1,
    .stepFast = 0x10,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

extern s32 *gFile_MPEG_TAB;
extern void mpeg_play(s32 id);

static s32 sound_test_window_open = FALSE;
static s32 sfxID;
static s32 lastSfxID = -1;
static u32 lastSfxID2 = 0;
static s32 musicAction;
static s32 lastMusicAction = -1;
static s32 volume = 127;
static s32 mpegID;
static s32 numMPEGEntries = -1;
static s32 numMusicActionEntries = -1;

RECOMP_CALLBACK(".", my_debug_menu_event) void sound_test_menu_callback() {
    dbgui_menu_item("Sound Test", &sound_test_window_open);
}

RECOMP_CALLBACK(".", my_dbgui_event) void sound_test_dbgui_callback() {
    if (numMPEGEntries < 0) {
        numMPEGEntries = (get_file_size(MPEG_TAB) / 4) - 1;
    }
    if (numMusicActionEntries < 0) {
        numMusicActionEntries = get_file_size(MUSICACTIONS_BIN) / sizeof(MusicAction);
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
                    static MusicAction killAction = { .seqID = 0 };

                    if (dbgui_button("Stop Ambience 1")) {
                        killAction.playerNo = 0;
                        recomp_play_music_action(&killAction, 0, 0);
                    }
                    dbgui_same_line();
                    if (dbgui_button("Stop Ambience 2")) {
                        killAction.playerNo = 1;
                        recomp_play_music_action(&killAction, 0, 0);
                    }

                    if (dbgui_button("Stop Music 1")) {
                        killAction.playerNo = 2;
                        recomp_play_music_action(&killAction, 0, 0);
                    }
                    dbgui_same_line();
                    if (dbgui_button("Stop Music 2")) {
                        killAction.playerNo = 3;
                        recomp_play_music_action(&killAction, 0, 0);
                    }

                    dbgui_separator();

                    if (dbgui_begin_child("scroll")) {
                        static MusicAction selectedAction;

                        dbgui_set_next_item_width(100);
                        if (dbgui_input_int("Music Action ID", &musicAction)) {
                            if (musicAction < 0) musicAction = 0;
                            if (musicAction > numMusicActionEntries) musicAction = numMusicActionEntries;

                            if (musicAction > 0) {
                                queue_load_file_region_to_ptr(
                                    (void**)&selectedAction, MUSICACTIONS_BIN, 
                                    (musicAction - 1) * sizeof(MusicAction), sizeof(MusicAction));
                            }
                        }

                        if (dbgui_button("Play")) {
                            if (lastMusicAction != -1 && gDLL_5_AMSEQ->vtbl->is_set(NULL, lastMusicAction)) {
                                gDLL_5_AMSEQ->vtbl->free(NULL, lastMusicAction, 0, 0, 0);
                            }
                            gDLL_5_AMSEQ->vtbl->set(NULL, musicAction, 0, 0, 0);
                            lastMusicAction = musicAction;
                        }

                        if (dbgui_tree_node("Music Action Info")) {
                            if (musicAction == 0) {
                                dbgui_text("(none selected)");
                            } else {
                                dbgui_textf("unk0: %d", selectedAction.unk0);
                                dbgui_textf("unk1: %d", selectedAction.unk1);
                                dbgui_textf("distFalloffExp: %d", selectedAction.distFalloffExp);
                                dbgui_textf("unk3: %d", selectedAction.unk3);
                                dbgui_textf("distFalloffStart: %d", selectedAction.distFalloffStart);
                                dbgui_textf("distFalloffEnd: %d", selectedAction.distFalloffEnd);
                                dbgui_textf("unk8: %d", selectedAction.unk8);
                                dbgui_textf("unkC: %d", selectedAction.unkC);
                                dbgui_textf("playerNo: %d", selectedAction.playerNo);
                                dbgui_textf("seqID: %d", selectedAction.seqID);
                                dbgui_textf("volume: %d", selectedAction.volume);
                                dbgui_textf("bpm: %d", selectedAction.bpm);
                                dbgui_textf("unk14: %d", selectedAction.unk14);
                                dbgui_textf("fadeTimeDs: %d", selectedAction.fadeTimeDs);
                                dbgui_textf("unk16: 0x%X", selectedAction.unk16);
                                dbgui_textf("unk18: 0x%X", selectedAction.unk18);
                                dbgui_textf("unk1A: 0x%X", selectedAction.unk1A);
                                dbgui_textf("unk1C: %d", selectedAction.unk1C);
                            }
                            dbgui_tree_pop();
                        }

                        dbgui_text("Note: Music actions also control ambience.");

                        dbgui_separator();

                        dbgui_text("Custom Music Action:");
                        dbgui_push_str_id("custom_music_action");

                        static MusicAction musicAction = { 
                            .distFalloffStart = 0x000A, 
                            .distFalloffEnd = 0x0050, 
                            .playerNo = 2, 
                            .seqID = 0, 
                            .volume = 127, 
                            .bpm = 0, 
                            .fadeTimeDs = 1, 
                            .unk16 = -1, 
                            .unk18 = -1
                        };

                        s32 seqID = musicAction.seqID;
                        if (dbgui_input_int("Seq ID", &seqID)) {
                            if (seqID < 0) seqID = 0;
                            musicAction.seqID = seqID;
                        }
                        s32 playerNo = musicAction.playerNo;
                        static const char *playerNames[] = {"Ambience 1", "Ambience 2", "Music 1", "Music 2"};
                        if (dbgui_begin_combo("Seq Player", playerNames[playerNo])) {
                            for (s32 i = 0; i < 4; i++) {
                                if (dbgui_selectable(playerNames[i], playerNo == i)) {
                                    musicAction.playerNo = i;
                                }
                            }

                            dbgui_end_combo();
                        }
                        s32 volume = musicAction.volume;
                        if (dbgui_input_int("Volume", &volume)) {
                            if (volume < 0) volume = 0;
                            if (volume > 127) volume = 127;
                            musicAction.volume = volume;
                        }
                        s32 unk16 = musicAction.unk16;
                        if (dbgui_tree_node("Channels (Instruments)")) {
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
                            s32 bpm = musicAction.bpm;
                            if (dbgui_input_int("BPM", &bpm)) {
                                musicAction.bpm = bpm;
                            }
                            s32 fadeTimeDs = musicAction.fadeTimeDs;
                            if (dbgui_input_int("fadeTimeDs", &fadeTimeDs)) {
                                musicAction.fadeTimeDs = fadeTimeDs;
                            }
                            s32 unk18 = musicAction.unk18;
                            if (dbgui_tree_node("unk18")) {
                                if (dbgui_button("Reset")) {
                                    musicAction.unk18 = -1;
                                }
                                for (s32 k = 0; k < 16; k++) {
                                    s32 status = (1 << k) & unk18;
                                    if (dbgui_checkbox(recomp_sprintf_helper("%d", k), &status)) {
                                        if (status) {
                                            unk18 |= (1 << k);
                                        } else {
                                            unk18 &= ~(1 << k);
                                        }
                                        musicAction.unk18 = unk18;
                                    }
                                    if (((k + 1) % 4) != 0) {
                                        dbgui_same_line();
                                    }
                                }
                                
                                dbgui_tree_pop();
                            }
                            s32 unk1A = musicAction.unk1A;
                            if (dbgui_tree_node("unk1A")) {
                                if (dbgui_button("Reset")) {
                                    musicAction.unk1A = -1;
                                }
                                for (s32 k = 0; k < 16; k++) {
                                    s32 status = (1 << k) & unk1A;
                                    if (dbgui_checkbox(recomp_sprintf_helper("%d", k), &status)) {
                                        if (status) {
                                            unk1A |= (1 << k);
                                        } else {
                                            unk1A &= ~(1 << k);
                                        }
                                        musicAction.unk1A = unk1A;
                                    }
                                    if (((k + 1) % 4) != 0) {
                                        dbgui_same_line();
                                    }
                                }
                                
                                dbgui_tree_pop();
                            }
                            
                            dbgui_tree_pop();
                        }

                        if (musicAction.playerNo < 2) { // channel
                            if (musicAction.seqID > 38) musicAction.seqID = 38;
                        } else {
                            if (musicAction.seqID > 110) musicAction.seqID = 110;
                        }

                        if (musicAction.playerNo < 2 && musicAction.seqID == 3) {
                            dbgui_text("WARNING: Ambience seq 3 will crash partially\ninto playback!");
                        }

                        if (dbgui_button("Play")) {
                            recomp_play_music_action(&musicAction, 0, 0);
                        }

                        dbgui_pop_id();

                    }
                    dbgui_end_child();
                    
                    dbgui_end_tab_item();
                }

                if (dbgui_begin_tab_item("MPEG", NULL)) {
                    if (dbgui_input_int("ID", &mpegID)) {
                        if (mpegID < 0) mpegID = 0;
                        if (mpegID >= numMPEGEntries) mpegID = numMPEGEntries - 1;
                    }

                    if (dbgui_button("Play")) {
                        mpeg_play(mpegID);
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

extern s32 amseq_func_1E8C(MusicAction *action, s8 arg1, s32 arg2);

static s32 recomp_play_music_action(MusicAction *action, s8 arg1, s32 arg2) {
    return amseq_func_1E8C(action, arg1, arg2);
}
