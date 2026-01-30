#include "modding.h"

#include "recomputils.h"
#include "recompdata.h"
#include "repacker_common.h"
#include "fst_ext.h"
#include "util/list.h"

#include "PR/ultratypes.h"
#include "dlls/engine/5_amseq.h"
#include "sys/fs.h"

typedef struct {
    s32 actionIdx;
    MusicAction *action;
} MusicActionReplacement;

static s32 mActionOriginalCount;
static List mActionReplacements;
static U32ValueHashmapHandle mActionReplacementMap;

static void maction_list_element_free(void *element) {
    MusicActionReplacement *replacement = element;
    if (replacement->action != NULL) {
        recomp_free(replacement->action);
        replacement->action = NULL;
    }
}

REPACKER_ON_INIT_CALLBACK void music_actions_ext_init(void) {
    mActionOriginalCount = fst_ext_get_file_size(MUSICACTIONS_BIN) / sizeof(MusicAction);

    list_init(&mActionReplacements, sizeof(MusicActionReplacement), 0);
    list_set_element_free_callback(&mActionReplacements, maction_list_element_free);
    mActionReplacementMap = recomputil_create_u32_value_hashmap();
}

REPACKER_ON_COMMIT_CALLBACK void music_actions_ext_commit(void) {
    s32 numReplacements = list_get_length(&mActionReplacements);
    if (numReplacements > 0) {
        // Calculate new music action count
        s32 maxIndex = mActionOriginalCount - 1;
        for (s32 i = 0; i < numReplacements; i++) {
            MusicActionReplacement *replacement = list_get(&mActionReplacements, i);
            if (replacement->action != NULL && replacement->actionIdx > maxIndex) {
                maxIndex = replacement->actionIdx;
            }
        }
        s32 newCount = maxIndex + 1;

        // Alloc new MUSICACTIONS.bin
        void *newActions = recomp_alloc(newCount * sizeof(MusicAction));

        // Load original actions
        u32 originalBinSize = mActionOriginalCount * sizeof(MusicAction);
        fst_ext_read_from_file(MUSICACTIONS_BIN, newActions, 0, originalBinSize);
        if (newCount > mActionOriginalCount) {
            u32 addedSize = (newCount - mActionOriginalCount) * sizeof(MusicAction);
            bzero((u8*)newActions + originalBinSize, addedSize);
        }

        // Update with replacements
        for (s32 i = 0; i < numReplacements; i++) {
            MusicActionReplacement *replacement = list_get(&mActionReplacements, i);
            void *maction = (u8*)newActions + (replacement->actionIdx * sizeof(MusicAction));

            if (replacement->action != NULL) {
                bcopy(replacement->action, maction, sizeof(MusicAction));
            }
        }

        // Set new file
        fst_ext_set_file(MUSICACTIONS_BIN, newActions, newCount * sizeof(MusicAction), /*ownedByRepacker=*/TRUE);

        repacker_log("[repacker] Rebuilt MUSICACTIONS.bin.\n");
    }

    list_free(&mActionReplacements);
    recomputil_destroy_u32_value_hashmap(mActionReplacementMap);
}

static MusicActionReplacement *get_maction_replacement(s32 actionIdx) {
    u32 listIdx;
    if (!recomputil_u32_value_hashmap_get(mActionReplacementMap, actionIdx, &listIdx)) {
        MusicActionReplacement replacement = { .actionIdx = actionIdx, .action = NULL };
        listIdx = list_add(&mActionReplacements, &replacement);
        recomputil_u32_value_hashmap_insert(mActionReplacementMap, actionIdx, listIdx);
    }

    return list_get(&mActionReplacements, listIdx);
}

static void load_maction(s32 actionIdx, MusicActionReplacement *replacement) {
    if (replacement->action == NULL) {
        void *maction = recomp_alloc(sizeof(MusicAction));
        replacement->action = maction;

        if (actionIdx < mActionOriginalCount) {
            fst_ext_read_from_file(MUSICACTIONS_BIN, maction, 
                actionIdx * sizeof(MusicAction), sizeof(MusicAction));
        } else {
            bzero(maction, sizeof(MusicAction));
        }
    }
}

RECOMP_EXPORT void repacker_music_actions_set_replacement(s32 actionIdx, const void *data) {
    MusicActionReplacement *replacement = get_maction_replacement(actionIdx);
    if (replacement->action == NULL) {
        replacement->action = recomp_alloc(sizeof(MusicAction));
    }

    bcopy(data, replacement->action, sizeof(MusicAction));
}

RECOMP_EXPORT void* repacker_music_actions_get(s32 actionIdx) {
    MusicActionReplacement *replacement = get_maction_replacement(actionIdx);
    load_maction(actionIdx, replacement);

    return replacement->action;
}
