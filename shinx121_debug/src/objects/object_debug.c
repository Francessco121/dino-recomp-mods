#include "object_debug.h"

#include "dbgui.h"
#include "recomputils.h"

#include "../debug_common.h"

#include "game/objects/object.h"
#include "sys/dll.h"

static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

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

void object_edit_contents(Object *obj) {
    dbgui_textf("(obj address): %p", obj);

    dbgui_textf("Position: %f,%f,%f",
        obj->srt.transl.x, obj->srt.transl.y, obj->srt.transl.z);
    dbgui_textf("Rotation (y,p,r): %d,%d,%d", 
        obj->srt.yaw, obj->srt.pitch, obj->srt.roll);
    const static DbgUiInputFloatOptions scaleInputOptions = {
        .step = 0.01f,
        .stepFast = 0.1f
    };
    dbgui_input_float_ext("Scale", &obj->srt.scale, &scaleInputOptions);
    dbgui_input_short("Flags", &obj->srt.flags);

    dbgui_textf("positionMirror: %f,%f,%f",
        obj->positionMirror.x, obj->positionMirror.y, obj->positionMirror.z);

    dbgui_textf("Speed: %f,%f,%f", 
        obj->speed.x, obj->speed.y, obj->speed.z);

    if (obj->parent != NULL) {
        if (dbgui_tree_node("parent")) {
            object_edit_contents(obj->parent);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("parent: null");
    }
    
    dbgui_input_byte("unk_0x34", &obj->unk_0x34);
    dbgui_input_sbyte("matrixIdx", &obj->matrixIdx);
    dbgui_input_byte("unk_0x36", &obj->unk_0x36);
    dbgui_input_byte("unk_0x37", &obj->unk_0x37);
    dbgui_textf("next: %p", &obj->next);
    dbgui_textf("unk0x3c: %f", obj->unk0x3c);
    dbgui_textf("unk0x40: %f", obj->unk0x40);
    dbgui_textf("group: %u", obj->group);
    dbgui_textf("id: %u", obj->id);
    dbgui_textf("tabIdx: %u", obj->tabIdx);
    if (obj->setup != NULL) {
        if (dbgui_tree_node("setup")) {
            ObjSetup *objsetup = obj->setup;
            dbgui_textf("objId: %d", objsetup->objId);
            dbgui_textf("quarterSize: %d", objsetup->quarterSize);
            dbgui_textf("setup: %d", objsetup->setup);
            dbgui_textf("loadParamA: %d", objsetup->loadParamA);
            dbgui_textf("loadParamB: %d", objsetup->loadParamB);
            dbgui_textf("loadDistance: %d", objsetup->loadDistance);
            dbgui_textf("fadeDistance: %d", objsetup->fadeDistance);
            dbgui_textf("x: %f", objsetup->x);
            dbgui_textf("y: %f", objsetup->y);
            dbgui_textf("z: %f", objsetup->z);
            dbgui_textf("uID: %d", objsetup->uID);

            u32 size = objsetup->quarterSize << 2;
            u32 address = (u32)objsetup;
            address += sizeof(ObjSetup);
            size -= sizeof(ObjSetup);

            if (size > 0) {
                dbgui_text("Additional data:");

                while (size > 8) {
                    u8 *addr = (u8*)address;

                    dbgui_textf("%08X:  %02X %02X %02X %02X  %02X %02X %02X %02X", (u32)addr - (u32)objsetup, 
                        addr[0], addr[1], addr[2], addr[3],
                        addr[4], addr[5], addr[6], addr[7]);

                    address += 8;
                    size -= 8;
                }
                if (size > 0) {
                    u8 *addr = (u8*)address;
                    const u32 extraByteStrMaxLen = 8 + 3 + (4 * 3) + 1 + (4 * 3) + 1;
                    char extraByteStr[extraByteStrMaxLen];
                    char *extraByteStrPtr = extraByteStr;

                    recomp_sprintf(extraByteStrPtr, "%08X:  ", (u32)addr - (u32)objsetup);
                    extraByteStrPtr += 8 + 3;

                    for (u32 i = 0; i < size && i < 4; i++) {
                        recomp_sprintf(extraByteStrPtr, "%02X ", *addr);
                        extraByteStrPtr += 3;

                        addr++;
                        size--;
                    }

                    if (size > 0) {
                        *extraByteStrPtr = ' ';
                        extraByteStrPtr += 1;

                        for (u32 i = 0; i < size; i++) {
                            recomp_sprintf(extraByteStrPtr, "%02X ", *addr);
                            extraByteStrPtr += 3;

                            addr++;
                            size--;
                        }
                    }

                    extraByteStr[extraByteStrMaxLen - 1] = '\0';
                    dbgui_text(extraByteStr);
                }
            }

            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("setup: null");
    }
    if (obj->def != NULL) {
        if (dbgui_tree_node("def")) {
            ObjDef *def = obj->def;
            dbgui_textf("dll: %d", resolve_dll_number(def->dllID));
            dbgui_textf("numModels: %d", def->numModels);
            dbgui_textf("name: %s", def->name);

            if (def->pAttachPoints != NULL && def->numAttachPoints > 0) {
                if (dbgui_tree_node("pAttachPoints")) {
                    for (int i = 0; i < def->numAttachPoints; i++) {
                        if (dbgui_tree_node(recomp_sprintf_helper("[%d]", i))) {
                            dbgui_input_float("x", &def->pAttachPoints[i].pos.x);
                            dbgui_input_float("y", &def->pAttachPoints[i].pos.y);
                            dbgui_input_float("z", &def->pAttachPoints[i].pos.z);

                            dbgui_tree_pop();
                        }
                    }

                    dbgui_tree_pop();
                }
            }

            if (def->pModLines != NULL && def->modLineCount > 0) {
                if (dbgui_tree_node("pModLines")) {
                    for (int i = 0; i < def->modLineCount; i++) {
                        if (dbgui_tree_node(recomp_sprintf_helper("[%d]", i))) {
                            HitsLineCustom *line = (HitsLineCustom*)&def->pModLines[i];

                            dbgui_textf("A: %d, %d, %d", line->Ax, line->Ay, line->Az);
                            dbgui_textf("B: %d, %d, %d", line->Bx, line->By, line->Bz);
                            if (line->settingsA & 0x80) {
                                dbgui_textf("height: %d", line->height);
                            } else {
                                dbgui_textf("heightA: %d", line->heightA);
                                dbgui_textf("heightB: %d", line->heightB);
                            }
                            dbgui_textf("settingsA: %02X", line->settingsA);
                            dbgui_textf("settingsB: %02X", line->settingsB);
                            dbgui_textf("animatorID: %d", line->animatorID);

                            dbgui_tree_pop();
                        }
                    }

                    dbgui_tree_pop();
                }
            }

            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("def: null");
    }
    if (obj->objhitInfo != NULL) {
        if (dbgui_tree_node("objhitInfo")) {
            ObjectHitInfo *objhitInfo = obj->objhitInfo;
            if (objhitInfo->unk_0x0 != NULL) {
                if (dbgui_tree_node("unk_0x0")) {
                    object_edit_contents(objhitInfo->unk_0x0);
                    dbgui_tree_pop();
                }
            } else {
                dbgui_textf("unk_0x0: null");
            }
            dbgui_textf("unk_0x4: %d", objhitInfo->unk_0x4);
            dbgui_textf("unk_0x6: %d", objhitInfo->unk_0x6);
            dbgui_textf("unk_0x8: %p", objhitInfo->unk_0x8);
            dbgui_textf("unk_0xC: %f", objhitInfo->unk_0xc);
            dbgui_textf("unk_0x10: %f,%f,%f", 
                objhitInfo->unk_0x10.x, objhitInfo->unk_0x10.y, objhitInfo->unk_0x10.z);
            dbgui_textf("unk_0x1c: %f", objhitInfo->unk_0x1c);
            dbgui_textf("unk_0x20: %f,%f,%f", 
                objhitInfo->unk_0x20.x, objhitInfo->unk_0x20.y, objhitInfo->unk_0x20.z);
            dbgui_textf("unk_0x2c: %f", objhitInfo->unk_0x2c);
            dbgui_textf("unk_0x30: %f", objhitInfo->unk_0x30);
            dbgui_textf("unk_0x34: %f", objhitInfo->unk_0x34);
            dbgui_textf("unk_0x38: %f", objhitInfo->unk_0x38);
            dbgui_textf("unk_0x3c: %f", objhitInfo->unk_0x3c);
            dbgui_textf("unk_0x40: %d", objhitInfo->unk_0x40);
            dbgui_textf("unk_0x44: %d", objhitInfo->unk_0x44);
            dbgui_textf("unk_0x48: %d", objhitInfo->unk_0x48);
            dbgui_textf("unk_0x4c: %d", objhitInfo->unk_0x4c);
            dbgui_textf("unk_0x50: %d", objhitInfo->unk_0x50);
            dbgui_textf("unk_0x52: %d", objhitInfo->unk_0x52);
            dbgui_textf("unk_0x54: %d", objhitInfo->unk_0x54);
            dbgui_textf("unk_0x56: %d", objhitInfo->unk_0x56);
            dbgui_input_short_ext("unk_0x58", &objhitInfo->unk_0x58, &hexInput);
            dbgui_input_byte_ext("unk_0x5a", &objhitInfo->unk_0x5a, &hexInput);
            dbgui_textf("unk_0x5f: %d", objhitInfo->unk_0x5f);
            dbgui_textf("unk_0x5b: %d", objhitInfo->unk_0x5b);
            dbgui_textf("unk_0x5c: %d", objhitInfo->unk_0x5c);
            dbgui_textf("unk_0x5d: %d", objhitInfo->unk_0x5d);
            dbgui_textf("unk_0x5e: %d", objhitInfo->unk_0x5e);
            dbgui_textf("unk_0x5f: %d", objhitInfo->unk_0x5f);
            dbgui_textf("unk_0x60: %d", objhitInfo->unk_0x60);
            dbgui_textf("unk_0x61: %d", objhitInfo->unk_0x61);
            dbgui_textf("unk_0x62: %d", objhitInfo->unk_0x62);
            dbgui_textf("unk_0x63: %d,%d,%d", objhitInfo->unk_0x63[0], objhitInfo->unk_0x63[1], objhitInfo->unk_0x63[2]);
            dbgui_textf("unk_0x66: %d,%d,%d", objhitInfo->unk_0x66[0], objhitInfo->unk_0x66[1], objhitInfo->unk_0x66[2]);
            dbgui_textf("unk_0x69: %d,%d,%d", objhitInfo->unk_0x69[0], objhitInfo->unk_0x69[1], objhitInfo->unk_0x69[2]);
            dbgui_textf("unk_0x6c: %d,%d,%d", objhitInfo->unk_0x6c[0], objhitInfo->unk_0x6c[1], objhitInfo->unk_0x6c[2]);
            dbgui_textf("unk_0x78: %f,%f,%f", objhitInfo->unk_0x78[0], objhitInfo->unk_0x78[1], objhitInfo->unk_0x78[2]);
            dbgui_textf("unk_0x84: %f,%f,%f", objhitInfo->unk_0x84[0], objhitInfo->unk_0x84[1], objhitInfo->unk_0x84[2]);
            dbgui_textf("unk_0x90: %f,%f,%f", objhitInfo->unk_0x90[0], objhitInfo->unk_0x90[1], objhitInfo->unk_0x90[2]);
            dbgui_textf("unk_0x9c: %d", objhitInfo->unk_0x9c);
            dbgui_textf("unk_0x9d: %d", objhitInfo->unk_0x9d);
            dbgui_textf("unk_0x9e: %d", objhitInfo->unk_0x9e);
            dbgui_textf("unk_0x9f: %d", objhitInfo->unk_0x9f);
            dbgui_textf("unk_0xa0: %d", objhitInfo->unk_0xa0);
            dbgui_textf("unk_0xa1: %d", objhitInfo->unk_0xa1);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("objhitInfo: null");
    }
    dbgui_textf("unk0x58: %d", obj->unk0x58);
    dbgui_textf("ptr0x5c: %p", obj->ptr0x5c);
    dbgui_textf("curEvent: %p", obj->curEvent);
    dbgui_textf("ptr0x64: %p", obj->ptr0x64);

    dbgui_textf("dll: %p", obj->dll);
    dbgui_textf("ptr0x6c: %p", obj->ptr0x6c);
    dbgui_textf("ptr0x70: %p", obj->ptr0x70);
    dbgui_textf("unk0x74: %d", obj->unk0x74);
    if (obj->unk_0x78 != NULL) {
        if (dbgui_tree_node("unk_0x78")) {
            ObjectStruct78 *unk_0x78 = obj->unk_0x78;
            dbgui_textf("unk0: %u", unk_0x78->unk0);
            dbgui_textf("unk1: %u", unk_0x78->unk1);
            dbgui_textf("unk2: %u", unk_0x78->unk2);
            dbgui_textf("unk3: %u", unk_0x78->unk3);
            dbgui_textf("unk4: %u", unk_0x78->unk4);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("unk_0x78: null");
    }
    dbgui_textf("positionMirror2: %f,%f,%f",
        obj->positionMirror2.x, obj->positionMirror2.y, obj->positionMirror2.z);
    dbgui_textf("positionMirror3: %f,%f,%f",
        obj->positionMirror3.x, obj->positionMirror3.y, obj->positionMirror3.z);
    dbgui_textf("animProgress: %f", obj->animProgress);
    dbgui_textf("animProgressLayered: %f", obj->animProgressLayered);
    dbgui_textf("curModAnimId: %d", obj->curModAnimId);
    dbgui_textf("curModAnimIdLayered: %d", obj->curModAnimIdLayered);
    dbgui_textf("unk_0xa4: %f", obj->unk_0xa4);
    dbgui_textf("unk_0xa8: %f", obj->unk_0xa8);
    dbgui_textf("mapID: %d", obj->mapID);
    if (dbgui_input_sbyte("modelInstIdx", &obj->modelInstIdx)) {
        // Game will crash if this goes out of bounds
        if (obj->modelInstIdx < 0) {
            obj->modelInstIdx = 0;
        } else if (obj->modelInstIdx >= obj->def->numModels) {
            obj->modelInstIdx = obj->def->numModels - 1;
        }
    }
    dbgui_textf("updatePriority: %d", obj->updatePriority);
    dbgui_textf("unk0xaf: %d", obj->unk0xaf);
    dbgui_textf("unk0xb0: %u", obj->unk0xb0);
    dbgui_textf("unk0xb2: %u", obj->unk0xb2);
    dbgui_textf("unk0xb4: %u", obj->unk0xb4);
    dbgui_textf("data: %p", obj->data);
    dbgui_textf("unk0xbc: %p", obj->unk0xbc);
    if (obj->unk0xc0 != NULL) {
        if (dbgui_tree_node("unk0xc0")) {
            object_edit_contents(obj->unk0xc0);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("unk0xc0: null");
    }
    dbgui_textf("unk0xc4: %u", obj->unk0xc4);
    if (obj->linkedObject != NULL) {
        if (dbgui_tree_node("linkedObject")) {
            object_edit_contents(obj->linkedObject);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("linkedObject: null");
    }
    if (obj->mesgQueue != NULL) {
        if (dbgui_tree_node("mesgQueue")) {
            ObjectMesgQueue *mesgQueue = obj->mesgQueue;
            dbgui_textf("count: %u", mesgQueue->count);
            dbgui_textf("capacity: %u", mesgQueue->capacity);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("mesgQueue: null");
    }
    dbgui_textf("unk_0xd4: %u", obj->unk_0xd4);
    dbgui_textf("unk_0xd6: %u", obj->unk_0xd6);
    dbgui_textf("unk_0xd8: %u", obj->unk_0xd8);
    dbgui_textf("unk_0xd9: %u", obj->unk_0xd9);
    dbgui_textf("unk_0xda: %u", obj->unk_0xda);
    dbgui_textf("unk0xdc: %d", obj->unk0xdc);
}
