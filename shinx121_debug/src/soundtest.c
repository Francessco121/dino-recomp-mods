#include "modding.h"
#include "dbgui.h"
#include "common.h"
#include "sys/fs.h"
#include "dll.h"

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

                if (dbgui_begin_tab_item("AMSEQ2 (Music)", NULL)) {
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
                    dbgui_same_line();
                    if (dbgui_button("Stop")) {
                        if (lastMusicAction != -1 && gDLL_5_AMSEQ->vtbl->func2(NULL, lastMusicAction)) {
                            gDLL_5_AMSEQ->vtbl->func1(NULL, lastMusicAction, 0, 0, 0);
                            lastMusicAction = -1;
                        }
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