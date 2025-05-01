#include "tab.h"

#include "imports.h"

#include "PR/ultratypes.h"
#include "sys/memory.h"

extern void bcopy_recomp(const void *src, void *dst, int length);
#define bcopy bcopy_recomp

extern void bzero_recomp(void *, int);
#define bzero bzero_recomp

extern void free_recomp(void* p);
#define free free_recomp

extern s32 __file1Address;

extern void read_from_rom(u32 romAddr, u8* dst, s32 size);

void tab_add_replacement(TabModList *list, s32 tabIdx, void *data, u32 size) {
    TabEntryReplacement replacement = { .tabIdx = tabIdx, .data = data, .size = size };
    list->replacements[list->replacementsLength++] = replacement;
}

s32 tab_find_idx(TabModList *list, u32 offset) {
    for (s32 i = list->tabLength - 1; i >= 0; i--) {
        if ((u32)list->tab[i] <= offset) {
            return i;
        }
    }

    return -1;
}

void tab_read(TabModList *list, s32 romAddr, void *dst, u32 offset, u32 size) {
    // Find tab index of the file referenced by the offset
    s32 tabIdx = tab_find_idx(list, offset);
    if (tabIdx == -1) {
        recomp_eprintf("Couldn't find tab index for offset %x in %s. Bad things to follow!\n", offset, list->name);
        // We don't have the correct ROM address to read, just return all zeroes
        bzero(dst, size);
        return;
    }

    // Find the replacement for the file, if any
    void *replacement = NULL;
    for (s32 i = 0; i < list->replacementsLength; i++) {
        if (list->replacements[i].tabIdx == tabIdx) {
            replacement = list->replacements[i].data;
            break;
        }
    }

    if (replacement == NULL) {
        // Adjust ROM address back to original
        romAddr += list->relocs[tabIdx];
        read_from_rom(romAddr + (s32)&__file1Address, dst, size);
    } else {
        // Read from replacement file
        u32 relativeOffset = offset - list->tab[tabIdx];
        bcopy((void*)((u32)replacement + relativeOffset), dst, size);

        recomp_printf("Using replacement for %s[%d]\n", list->name, tabIdx);
    }
}

s32 *tab_init(TabModList *list, s32 *originalTab, s32 originalTabLength) {
    s32 tabLength = originalTabLength;
    list->tabLength = tabLength;

    // Collect replacements into an easy temp lookup table
    s32 *replacementsTab = recomp_alloc(tabLength * 4);
    bzero(replacementsTab, tabLength * 4);

    for (s32 i = 0; i < list->replacementsLength; i++) {
        replacementsTab[list->replacements[i].tabIdx] = list->replacements[i].size;
    }

    // Rebuild tab with replacements in mind
    s32 *relocs = recomp_alloc(tabLength * 4);
    list->relocs = relocs;

    s32 *newTab = malloc((tabLength * 4) + 2, ALLOC_TAG_FS_COL, NULL);
    list->tab = newTab;

    u32 newOffset = 0;

    for (s32 i = 0; i < tabLength; i++) {
        u32 originalOffset = originalTab[i];
        u32 originalSize = originalTab[i + 1] - originalOffset;

        newTab[i] = newOffset;
        // Calculate relocations to convert new ROM offsets to original ROM offsets for unmodified files
        relocs[i] = (s32)originalOffset - (s32)newOffset;

        if (replacementsTab[i] > 0) {
            newOffset += replacementsTab[i];
        } else {
            newOffset += originalSize;
        }
    }

    newTab[tabLength] = newOffset;
    newTab[tabLength + 1] = 0xFFFFFFFF;

    recomp_free(replacementsTab);
    free(originalTab);

    return newTab;
}
