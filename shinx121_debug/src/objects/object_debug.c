#include "object_debug.h"

#include "dbgui.h"
#include "dll.h"
#include "recomputils.h"

#include "../debug_common.h"
#include "../3d.h"

#include "game/objects/object.h"
#include "game/objects/object_def.h"
#include "sys/dll.h"
#include "sys/gfx/model.h"
#include "sys/objanim.h"

extern DLLTab *gFile_DLLS_TAB;

extern s32* gTextureCache;
extern s32 gNumCachedTextures;
extern u16* gFile_TEXTABLE;

static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

static u16 resolve_dll_number(u16 id) {
    if (id >= 0x8000) {
        id -= 0x8000;
        id += gFile_DLLS_TAB->header.bank4;
    } else if (id >= 0x2000) {
        id -= 0x2000;
        id += gFile_DLLS_TAB->header.bank2 + 1;
    } else if (id >= 0x1000) {
        id -= 0x1000;
        id += gFile_DLLS_TAB->header.bank1 + 1;
    }

    return id;
}

static void object_show_def(ObjDef *def) {
    dbgui_textf("dllID: 0x%X (idx %d)", def->dllID, resolve_dll_number(def->dllID));
    dbgui_textf("name: %s", def->name);
    dbgui_separator();
    dbgui_textf("shadowScale: %f", def->shadowScale);
    dbgui_textf("scale: %f", def->scale);
    dbgui_textf("pModelList: %p", def->pModelList);
    dbgui_textf("pTextures: %p", def->pTextures);
    dbgui_textf("pSequenceBones: %p", def->pSequenceBones);
    dbgui_textf("unk14: %p", def->unk14);
    if (def->collectableDef != NULL) {
        if (dbgui_tree_node("collectableDef")) {
            CollectableDef *coldef = def->collectableDef;
            dbgui_textf("unk0: %d", coldef->unk0);
            dbgui_textf("type: %d", coldef->type);
            dbgui_textf("unk4: %d", coldef->unk4);
            dbgui_textf("seqObjectID: %d", coldef->seqObjectID);
            dbgui_textf("interactionRadius: %d", coldef->interactionRadius);
            dbgui_textf("collectMessage: %d", coldef->collectMessage);
            dbgui_textf("unkA: %d", coldef->unkA);
            dbgui_textf("amountRestored: %d", coldef->amountRestored);

            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("collectableDef: (null)");
    }
    if (def->pSeq != NULL) {
        if (dbgui_tree_node("pSeq")) {
            for (int i = 0; i < def->numSequences; i++) {
                dbgui_textf("[%x]: %d", i, def->pSeq[i]);
            }

            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("pSeq: (null)");
    }
    dbgui_textf("pEvent: %p", def->pEvent);
    dbgui_textf("pHits: %p", def->pHits);
    dbgui_textf("pWeaponData: %p", def->pWeaponData);
    dbgui_textf("pAttachPoints: %p", def->pAttachPoints);
    dbgui_textf("pModLines: %p", def->pModLines);
    dbgui_textf("pIntersectPoints: %p", def->pIntersectPoints);
    dbgui_textf("nextIntersectPoint: %p", def->nextIntersectPoint);
    dbgui_textf("nextIntersectLine: %p", def->nextIntersectLine);
    if (def->lockdata != NULL) {
        if (dbgui_tree_node("lockdata")) {
            for (int i = 0; i < def->numLockdata; i++) {
                if (dbgui_tree_node(recomp_sprintf_helper("[%d]", i))) {
                    ObjDefLockData *lockData = &def->lockdata[i];
                    dbgui_textf("unk00: %d", lockData->unk00);
                    dbgui_textf("unk02: %d", lockData->unk02);
                    dbgui_textf("unk04: %d", lockData->unk04);
                    dbgui_textf("unk06: %d", lockData->unk06);
                    dbgui_textf("unk08: %d", lockData->unk08);
                    dbgui_textf("unk0a: %d", lockData->unk0a);
                    dbgui_textf("interactRadius: %d", lockData->interactRadius);
                    dbgui_textf("lockExitRadius: %d", lockData->lockExitRadius);
                    dbgui_textf("hlRadius: %d", lockData->hlRadius);
                    dbgui_textf("hlAngularRange: %d", lockData->hlAngularRange);
                    dbgui_textf("flags: 0x%X", lockData->flags);
                    dbgui_textf("unk11[0]: %d", lockData->unk11[0]);
                    dbgui_textf("unk11[1]: %d", lockData->unk11[1]);
                    dbgui_textf("unk11[2]: %d", lockData->unk11[2]);
                    dbgui_textf("unk11[3]: %d", lockData->unk11[3]);
                    dbgui_textf("unk15: %d", lockData->unk15);
                    dbgui_textf("unk16: %d", lockData->unk16);
                    dbgui_textf("unk17: %d", lockData->unk17);

                    dbgui_tree_pop();
                }
            }

            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("lockdata: (null)");
    }
    dbgui_input_uint_ext("flags", &def->flags, &hexInput);
    if (def->shadowType != 0) {
        s32 type = def->shadowType;
        if (dbgui_input_int("shadowType", &type)) {
            if (type > 0) {
                def->shadowType = type;
            }
        }
    } else {
        dbgui_textf("shadowType: %d", def->shadowType);
    }
    dbgui_textf("shadowTexture: %d", def->shadowTexture);
    dbgui_textf("unk4C: %d", def->unk4C);
    dbgui_textf("unk4D: %d", def->unk4D);
    dbgui_textf("hitbox_flags60: 0x%X", def->hitbox_flags60);
    dbgui_textf("unk50: %d", def->unk50);
    dbgui_textf("unk52: %d", def->unk52);
    dbgui_textf("unk53: %d", def->unk53);
    dbgui_textf("unk54: %d", def->unk54);
    dbgui_textf("unk55: %d", def->unk55);
    dbgui_textf("numPlayerObjs: %d", def->numPlayerObjs);
    dbgui_textf("unk57: %d", def->unk57);
    dbgui_textf("group: %d", def->group);
    dbgui_textf("modLinesSize: %d", def->modLinesSize);
    dbgui_textf("numModels: %d", def->numModels);
    dbgui_textf("unk5e: %d", def->unk5e);

    if (def->pAttachPoints != NULL) {
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
    } else {
        dbgui_textf("pAttachPoints: (null)");
    }

    dbgui_textf("numAnimatedFrames: %d", def->numAnimatedFrames);
    dbgui_textf("numSequenceBones: %d", def->numSequenceBones);
    dbgui_textf("stateVar73: %d", def->stateVar73);
    dbgui_textf("unk74: %d", def->unk74);
    dbgui_textf("unk75: %d", def->unk75);
    dbgui_textf("modLineCount: %d", def->modLineCount);
    dbgui_textf("modLineNo: %d", def->modLineNo);
    dbgui_textf("numSequences: %d", def->numSequences);

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

    dbgui_textf("unk98: %d", def->_unk98[0]);
    dbgui_textf("unk99: %d", def->_unk98[1]);
    dbgui_textf("unk9A: %d", def->_unk98[2]);
}

static s32 get_texture_id(Texture *ptr) {
    for (int i = 0; i < gNumCachedTextures; i++) {
        if (ptr == (Texture*)gTextureCache[ASSETCACHE_PTR(i)]) {
            s32 id = gTextureCache[ASSETCACHE_ID(i)];
            if (id < 0) {
                id = -id;
            } else {
                id = gFile_TEXTABLE[id];
            }

            return id;
        }
    }

    return -1;
}

static void object_edit_model(Model *model) {
    dbgui_textf("(model address): %p", model);

    if (model->materials != NULL) {
        if (dbgui_tree_node("materials")) {
            for (int i = 0; i < model->textureCount; i++) {
                if (dbgui_tree_node(recomp_sprintf_helper("[%d]", i))) {
                    ModelTexture *mat = &model->materials[i];
                    dbgui_textf("texture: 0x%X", get_texture_id(mat->texture));
                    dbgui_textf("unk4: %d", mat->unk4);
                    dbgui_textf("unk6: %d", mat->pad6);
                    dbgui_textf("unk7: %d", mat->unk7);

                    dbgui_tree_pop();
                }
            }

            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("materials: (null)");
    }
    dbgui_textf("vertices: %p", model->vertices);
    dbgui_textf("faces: %p", model->faces);
    dbgui_textf("displayList: %p", model->displayList);
    dbgui_textf("anims: %p", model->anims);
    dbgui_textf("vertexGroupOffsets: %p", model->vertexGroupOffsets);
    dbgui_textf("vertexGroups: %p", model->vertexGroups);
    dbgui_textf("blendshapes: %p", model->blendshapes);
    dbgui_textf("joints: %p", model->joints);
    dbgui_textf("amap: %p", model->amap);
    dbgui_textf("hitSpheres: %p", model->hitSpheres);
    dbgui_textf("edgeVectors: %p", model->edgeVectors);
    dbgui_textf("modAnim: %p", model->modAnim);
    dbgui_textf("facebatchBounds: %p", model->facebatchBounds);
    dbgui_textf("drawModes: %p", model->drawModes);
    dbgui_textf("textureAnimations: %p", model->textureAnimations);
    for (s32 i = 0; i < 8; i++) {
        dbgui_textf("modAnimBankBases[%d]: %d", model->modAnimBankBases[i]);
    }
    dbgui_textf("collisionA: %p", model->collisionA);
    dbgui_textf("collisionB: %p", model->collisionB);
    dbgui_textf("decompressedSize: %d", model->decompressedSize);
    dbgui_textf("unk5C: %d", model->unk5C);
    dbgui_textf("maxAnimatedVertDistance: %d", model->maxAnimatedVertDistance);
    dbgui_textf("vertexCount: %d", model->vertexCount);
    dbgui_textf("faceCount: %d", model->faceCount);
    dbgui_textf("animCount: %d", model->animCount);
    dbgui_textf("unk68: %d", model->unk68);
    dbgui_textf("modelId: %d", model->modelId);
    dbgui_textf("displayListLength: %d", model->displayListLength);
    dbgui_textf("hitSphereCount: %d", model->hitSphereCount);
    dbgui_textf("jointCount: %d", model->jointCount);
    dbgui_textf("unk70: %d", model->unk70);
    dbgui_textf("unk71: %d", model->unk71);
    dbgui_textf("refCount: %d", model->refCount);
    dbgui_textf("textureCount: %d", model->textureCount);
    dbgui_textf("envMapCount: %d", model->envMapCount);
    dbgui_textf("drawModesCount: %d", model->drawModesCount);
    dbgui_textf("textureAnimationCount: %d", model->textureAnimationCount);
    dbgui_textf("unk77: %d", model->unk77);
    dbgui_textf("unk78: %d", model->unk78);
    dbgui_textf("unk7c: %d", model->unk7c);
}

static void object_draw_skeleton_hits(ModelInstance *modelInst, _Bool drawBoxes, _Bool drawBroadHits) {
    if (modelInst->unk14 == NULL || modelInst->model == NULL) {
        return;
    }
    ModelInstance_0x14_0x14 *sp50[100];
    bzero(sp50, sizeof(sp50));
    ModelInstance_0x14_0x14 arg3[100];
    bzero(arg3, sizeof(arg3));
    ModelInstance_0x14_0x14 arg4 = {0};
    static u32 colors[] = {
        0xFF3d0a0e,
        0xFFd48e21,
        0xFFfa1f99,
        0xFF09069e,
        0xFF03c03d,
        0xFF2d78ba,
        0xFFb78af5,
        0xFFdf2b6a,
        0xFF3143d3,
        0xFFe8a284,
        0xFF824c62,
        0xFF1ce580,
        0xFFb057fa,
        0xFFfafe6e,
        0xFF5ea52b,
        0xFFdc66a6
    };
    
    if (drawBroadHits) {
        func_8002F498(modelInst->unk14->unk0, modelInst->unk14, modelInst->model, arg3, &arg4);

        for (s32 k = 0; k < 20; k++) {
            ModelInstance_0x14_0x14 *thing = &arg3[k];
            if (thing->unk0[0] == -1) {
                break;
            }

            u32 color = colors[k % ARRAYCOUNT(colors)];

            s32 idx = thing->unk30 - 1;
            while (idx >= 0) {
                s32 joint = thing->unk0[idx];
                s32 parentIdx = modelInst->model->joints[joint].parentJointID;
                while (parentIdx != thing->unkC[idx]) {
                    Vec3f jointPos = {
                        modelInst->unk14->unk0[joint].x + gWorldX, 
                        modelInst->unk14->unk0[joint].y, 
                        modelInst->unk14->unk0[joint].z + gWorldZ
                    };

                    Vec3f parentPos = {
                        modelInst->unk14->unk0[parentIdx].x + gWorldX, 
                        modelInst->unk14->unk0[parentIdx].y, 
                        modelInst->unk14->unk0[parentIdx].z + gWorldZ
                    };

                    draw_3d_text(
                        jointPos.x, 
                        jointPos.y,
                        jointPos.z,
                        recomp_sprintf_helper("%d (%d[%d])", joint, k, idx), 
                        color);

                    draw_3d_line(jointPos.x, jointPos.y, jointPos.z, parentPos.x, parentPos.y, parentPos.z, color);

                    joint = parentIdx;
                    parentIdx = modelInst->model->joints[joint].parentJointID;
                }
                
                idx--;
            }

            SRT srt = {
                .yaw = 0,
                .pitch = 0,
                .roll = 0,
                .flags = 0,
                .scale = 1.0f,
                .transl = {
                    ((thing->unk18.x + thing->unk24.x) / 2.0f) + gWorldX,
                    ((thing->unk18.y + thing->unk24.y) / 2.0f) - ((thing->unk24.y - thing->unk18.y) / 2.0f),
                    ((thing->unk18.z + thing->unk24.z) / 2.0f) + gWorldZ
                }
            };
            MtxF mtx;
            matrix_from_srt(&mtx, &srt);

            draw_3d_box(&mtx, 
                thing->unk24.x - thing->unk18.x, 
                thing->unk24.y - thing->unk18.y, 
                thing->unk24.z - thing->unk18.z, 
                color);
        }
    }

    for (s32 k = 0; k < modelInst->model->jointCount; k++) {
        s32 parentIdx = modelInst->model->joints[k].parentJointID;

        if (drawBroadHits) {
            draw_3d_text(
                modelInst->unk14->unk0[k].x + gWorldX, 
                modelInst->unk14->unk0[k].y, 
                modelInst->unk14->unk0[k].z + gWorldZ, 
                recomp_sprintf_helper("\n%d", k), 
                0xFFFFFFFF);
        } else {
            draw_3d_text(
                modelInst->unk14->unk0[k].x + gWorldX, 
                modelInst->unk14->unk0[k].y, 
                modelInst->unk14->unk0[k].z + gWorldZ, 
                recomp_sprintf_helper("%d (%d)", k, parentIdx), 
                0xFFFFFFFF);
        }

        if (drawBoxes) {
            f32 radius = modelInst->unk14->unk4[k];
            SRT srt = {
                .yaw = 0,
                .pitch = 0,
                .roll = 0,
                .flags = 0,
                .scale = 1.0f,
                .transl = {
                    modelInst->unk14->unk0[k].x + gWorldX, 
                    modelInst->unk14->unk0[k].y - radius, 
                    modelInst->unk14->unk0[k].z + gWorldZ
                }
            };
            MtxF mtx;
            matrix_from_srt(&mtx, &srt);

            draw_3d_box(&mtx, radius*2, radius*2, radius*2, 0xFFFFFFFF);
        }
        
        if (!drawBroadHits) {
            Vec3f pos = {
                modelInst->unk14->unk0[k].x + gWorldX, 
                modelInst->unk14->unk0[k].y, 
                modelInst->unk14->unk0[k].z + gWorldZ
            };

            if (parentIdx >= 0) {
                Vec3f parent = {
                    modelInst->unk14->unk0[parentIdx].x + gWorldX, 
                    modelInst->unk14->unk0[parentIdx].y, 
                    modelInst->unk14->unk0[parentIdx].z + gWorldZ
                };

                draw_3d_line(pos.x, pos.y, pos.z, parent.x, parent.y, parent.z, 0xFFFFFFFF);
            }
        }
    }
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
    dbgui_input_short_ext("Flags", &obj->srt.flags, &hexInput);

    dbgui_textf("globalPosition: %f,%f,%f",
        obj->globalPosition.x, obj->globalPosition.y, obj->globalPosition.z);

    dbgui_textf("velocity: %f,%f,%f", 
        obj->velocity.x, obj->velocity.y, obj->velocity.z);

    if (obj->parent != NULL) {
        if (dbgui_tree_node("parent")) {
            object_edit_contents(obj->parent);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("parent: null");
    }
    
    dbgui_textf("unk34: %d", obj->unk34);
    dbgui_textf("matrixIdx: %d", obj->matrixIdx);
    dbgui_input_byte("opacity", &obj->opacity);
    dbgui_textf("opacityWithFade: %d", obj->opacityWithFade);
    dbgui_textf("next: %p", &obj->next);
    dbgui_textf("loadDistance: %f", obj->loadDistance);
    dbgui_textf("fadeDistance: %f", obj->fadeDistance);
    dbgui_textf("group: %u", obj->group);
    dbgui_textf("id: %u", obj->id);
    dbgui_textf("tabIdx: %u", obj->tabIdx);
    if (obj->setup != NULL) {
        if (dbgui_tree_node("setup")) {
            ObjSetup *objsetup = obj->setup;
            dbgui_textf("objId: %d", objsetup->objId);
            dbgui_textf("quarterSize: 0x%X (full size: 0x%X)", objsetup->quarterSize, objsetup->quarterSize << 2);
            dbgui_textf("actExclusions1: 0x%X", objsetup->actExclusions1);
            dbgui_textf("loadFlags: 0x%X", objsetup->loadFlags);
            dbgui_textf("actExclusions2: 0x%X", objsetup->actExclusions2 & 0xF0);
            dbgui_textf("fadeFlags: 0x%X", objsetup->fadeFlags & 0x0F);
            if (objsetup->loadFlags & 0x10) {
                dbgui_textf("mapObjGroup: %d", objsetup->mapObjGroup);
            } else {
                dbgui_textf("loadDistance: %d", objsetup->loadDistance);
            }
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

                while (size >= 8) {
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

                    recomp_sprintf(extraByteStrPtr, "%08X: ", (u32)addr - (u32)objsetup);
                    extraByteStrPtr += 8 + 2;

                    while (size > 0) {
                        *extraByteStrPtr = ' ';
                        extraByteStrPtr++;

                        u32 i = 0;
                        for (i = 0; i < size && i < 4; i++) {
                            recomp_sprintf(extraByteStrPtr, "%02X ", *addr);
                            extraByteStrPtr += 3;

                            addr++;
                        }

                        size -= i;
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
            object_show_def(def);

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
            dbgui_textf("hitTypeList: %d,%d,%d", objhitInfo->hitTypeList[0], objhitInfo->hitTypeList[1], objhitInfo->hitTypeList[2]);
            dbgui_textf("hitDamageList: %d,%d,%d", objhitInfo->hitDamageList[0], objhitInfo->hitDamageList[1], objhitInfo->hitDamageList[2]);
            dbgui_textf("hitByList: %d,%d,%d", objhitInfo->hitByList[0], objhitInfo->hitByList[1], objhitInfo->hitByList[2]);
            dbgui_textf("hitXList: %f,%f,%f", objhitInfo->hitXList[0], objhitInfo->hitXList[1], objhitInfo->hitXList[2]);
            dbgui_textf("hitYList: %f,%f,%f", objhitInfo->hitYList[0], objhitInfo->hitYList[1], objhitInfo->hitYList[2]);
            dbgui_textf("hitZList: %f,%f,%f", objhitInfo->hitZList[0], objhitInfo->hitZList[1], objhitInfo->hitZList[2]);
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
    dbgui_textf("polyhits: %p", obj->polyhits);
    dbgui_textf("unk5C: %p", obj->unk5C);
    dbgui_textf("curEvent: %p", obj->curEvent);
     if (obj->shadow != NULL) {
        if (dbgui_tree_node("shadow")) {
            ObjectShadow *shadow = obj->shadow;
            dbgui_input_float("scale", &shadow->scale);
            dbgui_textf("texture: %p", shadow->texture);
            dbgui_textf("unk8: %p", shadow->unk8);
            dbgui_textf("gdl: %p", shadow->gdl);
            dbgui_textf("gdl2: %p", shadow->gdl2);
            dbgui_textf("dir: %f,%f,%f", shadow->dir.x, shadow->dir.y, shadow->dir.z);
            dbgui_textf("tr: %f,%f,%f", shadow->tr.x, shadow->tr.y, shadow->tr.z);
            dbgui_input_float("maxDistScale", &shadow->maxDistScale);
            if (dbgui_tree_node(recomp_sprintf_helper("flags: 0x%X###flags", shadow->flags))) {
                static s32 allowUnsafeTexSlotEdit = FALSE;
                dbgui_checkbox("Allow unsafe texture slot edits", &allowUnsafeTexSlotEdit);
                s32 texSlot = OBJ_SHADOW_FLAG_GET_TEX_SLOT(shadow->flags);
                if (dbgui_input_int("texSlot", &texSlot)) {
                    if (!allowUnsafeTexSlotEdit) {
                        texSlot &= 1;
                    }
                    shadow->flags &= ~3;
                    shadow->flags |= OBJ_SHADOW_FLAG_MAKE_TEX_SLOT(texSlot);
                }
                const char *flagNames[32] = {
                    /*texSlot*/"", "",
                    /*OBJ_SHADOW_FLAG_4*/"Enabled",
                    /*OBJ_SHADOW_FLAG_8*/"Dynamic shadow texture",
                    /*OBJ_SHADOW_FLAG_10*/"Custom shadow direction",
                    /*OBJ_SHADOW_FLAG_20*/"Custom objecet position",
                    /*OBJ_SHADOW_FLAG_40*/"Custom shadow color",
                    /*OBJ_SHADOW_FLAG_80*/"Disable Z-buffering",
                    /*OBJ_SHADOW_FLAG_100*/"",
                    /*OBJ_SHADOW_FLAG_200*/"Use object yaw",
                    /*OBJ_SHADOW_FLAG_400*/"Custom opacity",
                    /*OBJ_SHADOW_FLAG_800*/"Top down shadow",
                    /*OBJ_SHADOW_FLAG_1000*/"Fade out",
                    /*OBJ_SHADOW_FLAG_2000*/"Unused",
                    /*OBJ_SHADOW_FLAG_4000*/"Unused",
                    /*OBJ_SHADOW_FLAG_8000*/"",
                    /*OBJ_SHADOW_FLAG_10000*/"Prevent fade in",
                    /*OBJ_SHADOW_FLAG_20000*/"Unused",
                    /*OBJ_SHADOW_FLAG_40000*/"Water surface"
                };
                for (s32 k = 2; k < 19; k++) {
                    s32 status = (1 << k) & shadow->flags;
                    const char *label = flagNames[k];
                    if (label == NULL) label = "";
                    if (dbgui_checkbox(recomp_sprintf_helper("0x%X %s###%d", (1 << k), label, k), &status)) {
                        if (status) {
                            shadow->flags |= (1 << k);
                        } else {
                            shadow->flags &= ~(1 << k);
                        }
                    }
                }
                dbgui_tree_pop();
            }
            dbgui_textf("visibility: %d", shadow->visibility);
            dbgui_input_byte("distFadeStart", &shadow->distFadeStart);
            dbgui_input_byte("distFadeEnd", &shadow->distFadeEnd);
            dbgui_input_byte("distFadeMaxOpacity", &shadow->distFadeMaxOpacity);
            dbgui_input_byte("distFadeMinOpacity", &shadow->distFadeMinOpacity);
            dbgui_input_byte("r", &shadow->r);
            dbgui_input_byte("g", &shadow->g);
            dbgui_input_byte("b", &shadow->b);
            dbgui_input_byte("a", &shadow->a);
            dbgui_input_byte("opacity", &shadow->opacity);
            dbgui_textf("bufferIdx: %d", shadow->bufferIdx);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("shadow: null");
    }

    dbgui_textf("dll: %p", obj->dll);
    dbgui_textf("unk6C: %p", obj->unk6C);
    dbgui_textf("unk70: %p", obj->unk70);
    dbgui_textf("unk74: %p", obj->unk74);
    if (obj->unk78 != NULL) {
        if (dbgui_tree_node("unk78")) {
            ObjectStruct78 *unk78 = obj->unk78;
            dbgui_textf("interactRadius: %u", unk78->interactRadius);
            dbgui_textf("lockExitRadius: %u", unk78->lockExitRadius);
            dbgui_textf("hlRadius: %u", unk78->hlRadius);
            dbgui_textf("hlAngularRange: %u", unk78->hlAngularRange);
            dbgui_textf("flags: %u", unk78->flags);
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("unk78: null");
    }
    if (obj->modelInsts != NULL) {
        if (dbgui_tree_node("modelInsts")) {
            ModelInstance **modelInsts = obj->modelInsts;
            if (obj->def != NULL) {
                for (s32 i = 0; i < obj->def->numModels; i++) {
                    ModelInstance *modelInst = modelInsts[i];
                    if (modelInst == NULL) {
                        dbgui_textf("[%d]: null", i);
                        continue;
                    }
                    if (dbgui_tree_node(recomp_sprintf_helper("[%d]", i))) {
                        if (modelInst->model != NULL && modelInst->unk14 != NULL) {
                            static s32 drawSkeleton = FALSE;
                            static s32 drawSkeletonHitboxes = FALSE;
                            static s32 drawSkeletonBroadHits = FALSE;
                            dbgui_checkbox("Draw skeleton hits", &drawSkeleton);
                            if (drawSkeleton) {
                                dbgui_checkbox("Draw skeleton hit boxes", &drawSkeletonHitboxes);
                                dbgui_checkbox("Draw skeleton broad hits", &drawSkeletonBroadHits);
                            }
                            if (drawSkeleton) {
                                object_draw_skeleton_hits(modelInst, drawSkeletonHitboxes, drawSkeletonBroadHits);
                            }
                        }

                        if (modelInst->model != NULL) {
                            if (dbgui_tree_node("model")) {
                                object_edit_model(modelInst->model);
                                dbgui_tree_pop();
                            }
                        } else {
                            dbgui_textf("model: null");
                        }
                        dbgui_textf("vertices[0]: %p", modelInst->vertices[0]);
                        dbgui_textf("vertices[1]: %p", modelInst->vertices[1]);
                        dbgui_textf("matrices[0]: %p", modelInst->matrices[0]);
                        dbgui_textf("matrices[1]: %p", modelInst->matrices[1]);
                        if (modelInst->unk14 != NULL) {
                            if (dbgui_tree_node("unk14")) {
                                ModelInstance_0x14 *unk14 = modelInst->unk14;
                                if (modelInst->model != NULL) {
                                    for (s32 k = 0; k < modelInst->model->jointCount; k++) {
                                        dbgui_textf("unk0[%d]: %f, %f, %f", k, unk14->unk0[k].x, unk14->unk0[k].y, unk14->unk0[k].z);
                                    }
                                    for (s32 k = 0; k < modelInst->model->jointCount; k++) {
                                        dbgui_textf("unk4[%d]: %f", k, unk14->unk4[k]);
                                    }
                                }
                                dbgui_tree_pop();
                            }
                        } else {
                            dbgui_textf("unk14: null");
                        }
                        dbgui_textf("displayList: %p", modelInst->displayList);
                        dbgui_textf("unk1C[0]: %p", modelInst->unk1C[0]);
                        dbgui_textf("unk1C[1]: %p", modelInst->unk1C[1]);
                        dbgui_textf("unk24: %p", modelInst->unk24);
                        dbgui_textf("animState0: %p", modelInst->animState0);
                        dbgui_textf("animState1: %p", modelInst->animState1);
                        dbgui_textf("blendshapes: %p", modelInst->blendshapes);
                        dbgui_textf("unk34: %d", modelInst->unk34);
                        dbgui_tree_pop();
                    }
                }
            }
            dbgui_tree_pop();
        }
    } else {
        dbgui_textf("modelInsts: null");
    }
    dbgui_textf("prevLocalPosition: %f,%f,%f",
        obj->prevLocalPosition.x, obj->prevLocalPosition.y, obj->prevLocalPosition.z);
    dbgui_textf("prevGlobalPosition: %f,%f,%f",
        obj->prevGlobalPosition.x, obj->prevGlobalPosition.y, obj->prevGlobalPosition.z);
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
    dbgui_input_byte_ext("unkAF", &obj->unkAF, &hexInput);
    dbgui_input_ushort_ext("unkB0", &obj->unkB0, &hexInput);
    dbgui_textf("unkB2: 0x%X", obj->unkB2);
    dbgui_textf("unkB4: 0x%X", obj->unkB4);
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
    dbgui_input_int("unkDC", &obj->unkDC);
}

void object_seq_debug(Object *obj, ObjEditorData *editorData) {
    ObjDef *def = obj->def;
    if (def == NULL || def->pSeq == NULL) {
        return;
    }

    s16 *seq = def->pSeq;
    s32 seqCount = def->numSequences;

    if (dbgui_begin_combo("Sequence", recomp_sprintf_helper("[%d] 0x%X", editorData->seqIdx, seq[editorData->seqIdx]))) {
        for (s32 i = 0; i < seqCount; i++) {
            if (dbgui_selectable(recomp_sprintf_helper("[%d] 0x%X", i, seq[i]), i == editorData->seqIdx)) {
                editorData->seqIdx = i;
            }
        }
        dbgui_end_combo();
    }

    dbgui_text("Actors");
    for (s32 k = 0; k < 16; k++) {
        s32 status = (editorData->seqActorBits & (1 << k)) ? 1 : 0;
        if (dbgui_checkbox(recomp_sprintf_helper("%d", k), &status)) {
            if (!status) {
                editorData->seqActorBits &= ~(1 << k);
            } else {
                editorData->seqActorBits |= (1 << k);
            }
        }
        if (((k + 1) % 4) != 0) {
            dbgui_same_line();
        }
    }

    if (dbgui_button("Play")) {
        editorData->seqPlayLastRet = gDLL_3_Animation->vtbl->func17(editorData->seqIdx, obj, editorData->seqActorBits);
    }
    
    dbgui_textf("Last play return value: %d", editorData->seqPlayLastRet);
}

void object_anim_debug(Object *obj, ObjEditorData *editorData) {
    ModelInstance *modelInst = obj->modelInsts[obj->modelInstIdx];
    if (modelInst == NULL) {
        return;
    }

    Model *model = modelInst->model;
    if (model == NULL) {
        return;
    }

    dbgui_textf("Num model animations: %d", model->animCount);

    dbgui_input_int("Model anim index", &editorData->modAnimIdx);
    if (editorData->modAnimIdx < 0) editorData->modAnimIdx = 0;
    if (editorData->modAnimIdx >= model->animCount) editorData->modAnimIdx = model->animCount - 1;

    if (dbgui_button("Play")) {
        func_80023D30(obj, editorData->modAnimIdx, 0.0f, 0);
    }
}
