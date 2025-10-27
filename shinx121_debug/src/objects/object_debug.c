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

    dbgui_textf("speed: %f,%f,%f", 
        obj->speed.x, obj->speed.y, obj->speed.z);

    if (obj->parent != NULL) {
        if (dbgui_tree_node("parent")) {
            object_edit_contents(obj->parent);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("parent: null");
    }
    
    dbgui_input_byte("unk34", &obj->unk34);
    dbgui_input_sbyte("matrixIdx", &obj->matrixIdx);
    dbgui_input_byte("unk36", &obj->unk36);
    dbgui_input_byte("unk37", &obj->unk37);
    dbgui_textf("next: %p", &obj->next);
    dbgui_textf("unk3C: %f", obj->unk3C);
    dbgui_textf("unk40: %f", obj->unk40);
    dbgui_textf("group: %u", obj->group);
    dbgui_textf("id: %u", obj->id);
    dbgui_textf("tabIdx: %u", obj->tabIdx);
    if (obj->setup != NULL) {
        if (dbgui_tree_node("setup")) {
            ObjSetup *objsetup = obj->setup;
            dbgui_textf("objId: %d", objsetup->objId);
            dbgui_textf("quarterSize: 0x%X", objsetup->quarterSize);
            dbgui_textf("setup: %d", objsetup->setup);
            dbgui_textf("loadParamA: %d", objsetup->loadParamA);
            dbgui_textf("loadParamB: %d", objsetup->loadParamB);
            dbgui_textf("loadDistance: %d", objsetup->loadDistance);
            dbgui_textf("fadeDistance: %d", objsetup->fadeDistance);
            dbgui_textf("x: %f", objsetup->x);
            dbgui_textf("y: %f", objsetup->y);
            dbgui_textf("z: %f", objsetup->z);
            dbgui_textf("uID: 0x%X", objsetup->uID);

            u32 size = objsetup->quarterSize << 2;
            u32 address = (u32)objsetup;
            address += sizeof(ObjSetup);

            if (size > sizeof(ObjSetup)) {
                size -= sizeof(ObjSetup);
                
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
            if (objhitInfo->unk0 != NULL) {
                if (dbgui_tree_node("unk0")) {
                    object_edit_contents(objhitInfo->unk0);
                    dbgui_tree_pop();
                }
            } else {
                dbgui_textf("unk0: null");
            }
            dbgui_textf("unk4: %d", objhitInfo->unk4);
            dbgui_textf("unk6: %d", objhitInfo->unk6);
            dbgui_textf("unk8: %p", objhitInfo->unk8);
            dbgui_textf("unkC: %f", objhitInfo->unkC);
            dbgui_textf("unk10: %f,%f,%f", 
                objhitInfo->unk10.x, objhitInfo->unk10.y, objhitInfo->unk10.z);
            dbgui_textf("unk1C: %f", objhitInfo->unk1C);
            dbgui_textf("unk20: %f,%f,%f", 
                objhitInfo->unk20.x, objhitInfo->unk20.y, objhitInfo->unk20.z);
            dbgui_textf("unk2C: %f", objhitInfo->unk2C);
            dbgui_textf("unk30: %f", objhitInfo->unk30);
            dbgui_textf("unk34: %f", objhitInfo->unk34);
            dbgui_textf("unk38: %f", objhitInfo->unk38);
            dbgui_textf("unk3C: %f", objhitInfo->unk3C);
            dbgui_textf("unk40: %d", objhitInfo->unk40);
            dbgui_textf("unk44: %d", objhitInfo->unk44);
            dbgui_textf("unk48: %d", objhitInfo->unk48);
            dbgui_textf("unk4C: %d", objhitInfo->unk4C);
            dbgui_textf("unk50: %d", objhitInfo->unk50);
            dbgui_textf("unk52: %d", objhitInfo->unk52);
            dbgui_textf("unk54: %d", objhitInfo->unk54);
            dbgui_textf("unk56: %d", objhitInfo->unk56);
            dbgui_input_short_ext("unk58", &objhitInfo->unk58, &hexInput);
            dbgui_input_byte_ext("unk5A", &objhitInfo->unk5A, &hexInput);
            dbgui_textf("unk5F: %d", objhitInfo->unk5F);
            dbgui_textf("unk5B: %d", objhitInfo->unk5B);
            dbgui_textf("unk5C: %d", objhitInfo->unk5C);
            dbgui_textf("unk5D: %d", objhitInfo->unk5D);
            dbgui_textf("unk5E: %d", objhitInfo->unk5E);
            dbgui_textf("unk5F: %d", objhitInfo->unk5F);
            dbgui_textf("unk60: %d", objhitInfo->unk60);
            dbgui_textf("unk61: %d", objhitInfo->unk61);
            dbgui_textf("unk62: %d", objhitInfo->unk62);
            dbgui_textf("unk63: %d,%d,%d", objhitInfo->unk63[0], objhitInfo->unk63[1], objhitInfo->unk63[2]);
            dbgui_textf("unk66: %d,%d,%d", objhitInfo->unk66[0], objhitInfo->unk66[1], objhitInfo->unk66[2]);
            dbgui_textf("unk69: %d,%d,%d", objhitInfo->unk69[0], objhitInfo->unk69[1], objhitInfo->unk69[2]);
            dbgui_textf("unk6C: %d,%d,%d", objhitInfo->unk6C[0], objhitInfo->unk6C[1], objhitInfo->unk6C[2]);
            dbgui_textf("unk78: %f,%f,%f", objhitInfo->unk78[0], objhitInfo->unk78[1], objhitInfo->unk78[2]);
            dbgui_textf("unk84: %f,%f,%f", objhitInfo->unk84[0], objhitInfo->unk84[1], objhitInfo->unk84[2]);
            dbgui_textf("unk90: %f,%f,%f", objhitInfo->unk90[0], objhitInfo->unk90[1], objhitInfo->unk90[2]);
            dbgui_textf("unk9C: %d", objhitInfo->unk9C);
            dbgui_textf("unk9D: %d", objhitInfo->unk9D);
            dbgui_textf("unk9E: %d", objhitInfo->unk9E);
            dbgui_textf("unk9F: %d", objhitInfo->unk9F);
            dbgui_textf("unkA0: %d", objhitInfo->unkA0);
            dbgui_textf("unkA1: %d", objhitInfo->unkA1);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("objhitInfo: null");
    }
    dbgui_textf("unk58: %d", obj->unk58);
    dbgui_textf("ptr0x5c: %p", obj->ptr0x5c);
    dbgui_textf("curEvent: %p", obj->curEvent);
    dbgui_textf("ptr0x64: %p", obj->ptr0x64);

    dbgui_textf("dll: %p", obj->dll);
    dbgui_textf("ptr0x6c: %p", obj->ptr0x6c);
    dbgui_textf("ptr0x70: %p", obj->ptr0x70);
    dbgui_textf("unk74: %d", obj->unk74);
    if (obj->unk78 != NULL) {
        if (dbgui_tree_node("unk78")) {
            ObjectStruct78 *unk78 = obj->unk78;
            dbgui_textf("unk0: %u", unk78->unk0);
            dbgui_textf("unk1: %u", unk78->unk1);
            dbgui_textf("unk2: %u", unk78->unk2);
            dbgui_textf("unk3: %u", unk78->unk3);
            dbgui_textf("unk4: %u", unk78->unk4);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("unk78: null");
    }
    dbgui_textf("positionMirror2: %f,%f,%f",
        obj->positionMirror2.x, obj->positionMirror2.y, obj->positionMirror2.z);
    dbgui_textf("positionMirror3: %f,%f,%f",
        obj->positionMirror3.x, obj->positionMirror3.y, obj->positionMirror3.z);
    dbgui_textf("animProgress: %f", obj->animProgress);
    dbgui_textf("animProgressLayered: %f", obj->animProgressLayered);
    dbgui_textf("curModAnimId: %d", obj->curModAnimId);
    dbgui_textf("curModAnimIdLayered: %d", obj->curModAnimIdLayered);
    dbgui_textf("unkA4: %f", obj->unkA4);
    dbgui_textf("unkA8: %f", obj->unkA8);
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
    dbgui_textf("unkAF: %d", obj->unkAF);
    dbgui_textf("unkB0: %u", obj->unkB0);
    dbgui_textf("unkB2: %u", obj->unkB2);
    dbgui_textf("unkB4: %u", obj->unkB4);
    dbgui_textf("data: %p", obj->data);
    dbgui_textf("animCallback: %p", obj->animCallback);
    if (obj->unkC0 != NULL) {
        if (dbgui_tree_node("unkC0")) {
            object_edit_contents(obj->unkC0);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("unkC0: null");
    }
    dbgui_textf("unkC4: %u", obj->unkC4);
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
    dbgui_textf("unkD4: %u", obj->unkD4);
    dbgui_textf("unkD6: %u", obj->unkD6);
    dbgui_textf("unkD8: %u", obj->unkD8);
    dbgui_textf("unkD9: %u", obj->unkD9);
    dbgui_textf("unkDA: %u", obj->unkDA);
    dbgui_textf("unkDC: %d", obj->unkDC);
}
