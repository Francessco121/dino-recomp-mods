#include "modding.h"
#include "imports.h"
#include "common.h"
#include "sys/objects.h"

void bcopy2(const void *src, void *dst, int size)
{
    char *dstc = (char *)dst;
    const char *srcc = (const char *)src;
    while (size > 0)
    {
        *dstc++ = *srcc++;
        size--;
    }
}

void bzero2(void *dst, int size)
{
    char *dstc = (char *)dst;
    while (size > 0)
    {
        *dstc++ = 0;
        size--;
    }
}

extern int gObjIndexCount; //count of OBJINDEX.BIN entries
extern s16  *gFile_OBJINDEX;
extern int gNumObjectsTabEntries;
extern ObjDef **gLoadedObjDefs;
extern u8 *gObjDefRefCount;
extern s32  *gFile_OBJECTS_TAB;

extern ObjDef *obj_load_objdef(s32 tabIdx);
extern u32 func_80022828(Object *obj);
extern u32 obj_calc_mem_size(Object *obj, ObjDef *def, u32 flags);
extern void obj_free_objdef(s32 tabIdx);
extern void func_80021E74(f32, ModelInstance*);
extern void func_80022200(Object *obj, s32 param2, s32 param3);
extern u32 obj_alloc_object_state(Object *obj, u32 addr);
extern u32 func_80022868(s32 param1, Object *obj, u32 addr);
extern u32 func_8002298C(s32 param1, ModelInstance *param2, Object *obj, u32 addr);
extern f32 func_80022150(Object *obj);
extern ModLine *obj_load_objdef_modlines(s32 modLineNo, s16 *modLineCount);
extern void func_800596BC(ObjDef*);

RECOMP_PATCH ObjDef *obj_load_objdef(s32 tabIdx) {
    ObjDef *def;
    s32 fileOffset;
    s32 fileSize;

    if (tabIdx >= gNumObjectsTabEntries) {
        recomp_eprintf("Failed to load objdef idx %d. Index out of bounds.\n", tabIdx);
        return NULL;
    }
    
    if (gObjDefRefCount[tabIdx] != 0) {
        gObjDefRefCount[tabIdx]++;
        def = gLoadedObjDefs[tabIdx];
        return def;
    }
    
    fileOffset = gFile_OBJECTS_TAB[tabIdx];
    fileSize = gFile_OBJECTS_TAB[tabIdx + 1] - fileOffset;

    def = (ObjDef*)malloc(fileSize, ALLOC_TAG_OBJECTS_COL, NULL);
    if (def != NULL) {
        read_file_region(OBJECTS_BIN, (void*)def, fileOffset, fileSize);

        if (def->pEvent != 0) {
            def->pEvent = (ObjDefEvent*)((u32)def + (u32)def->pEvent);
        }

        if (def->pHits != 0) {
            def->pHits = (ObjDefHit*)((u32)def + (u32)def->pHits);
        }

        if (def->pWeaponData != 0) {
            def->pWeaponData = (ObjDefWeaponData*)((u32)def + (u32)def->pWeaponData);
        }

        def->pModelList = (u32*)((u32)def + (u32)def->pModelList);
        def->pTextures = (UNK_PTR*)((u32)def + (u32)def->pTextures);
        def->unk10 = (UNK_PTR*)((u32)def + (u32)def->unk10);

        if (def->unk18 != 0) {
            def->unk18 = (u32*)((u32)def + (u32)def->unk18);
        }

        if (def->unk40 != 0) {
            def->unk40 = (ObjDefStruct40*)((u32)def + (u32)def->unk40);
        }

        if (def->pSeq != 0) {
            def->pSeq = (s16*)((u32)def + (u32)def->pSeq);
        }

        def->pModLines = NULL;

        def->pAttachPoints = (AttachPoint*)((u32)def + (u32)def->pAttachPoints);

        def->pIntersectPoints = NULL;

        if (def->modLineNo > -1) {
            def->pModLines = obj_load_objdef_modlines(def->modLineNo, &def->modLineCount);
            func_800596BC(def);
        }

        gLoadedObjDefs[tabIdx] = def;
        gObjDefRefCount[tabIdx] = 1;
    } else {
        recomp_eprintf("Failed to load objdef idx %d. Malloc failed.\n", tabIdx);
        return NULL;
    }

    return def;
}

RECOMP_PATCH Object *obj_setup_object(ObjCreateInfo *createInfo, u32 param2, s32 mapID, s32 param4, Object *parent, s32 param6) {
    ObjDef *def;
    s32 modelCount;
    s32 var;
    s32 flags;
    ModelInstance *tempModel;
    Object *obj;
    s32 tabIdx;
    s32 objId;
    s32 j;
    Object objHeader;
    s32 addr;
    s8 modelLoadFailed;

    objId = createInfo->objId;

    update_pi_manager_array(0, objId);

    if (param2 & 2) {
        tabIdx = objId;
    } else {
        if (objId > gObjIndexCount) {
            recomp_eprintf("Failed to setup object %d. Too many objects loaded.\n", objId);
            update_pi_manager_array(0, -1);
            return NULL;
        }

        tabIdx = gFile_OBJINDEX[objId];
    }

    bzero2(&objHeader, sizeof(Object));

    objHeader.def = obj_load_objdef(tabIdx);
    def = objHeader.def;

    if (def == NULL || (u32)def == 0xFFFFFFFF) {
        recomp_eprintf("Failed to setup object %d. Failed to load objdef.\n", objId);
        return NULL;
    } 
    
    objHeader.srt.flags = 2;

    if (def->flags & 0x80) {
        objHeader.srt.flags = 0x82;
    }

    if (def->flags & 0x40000) {
        objHeader.unk0xb0 |= 0x80;
    }

    if (param2 & 4) {
        objHeader.srt.flags |= 0x2000;
    }

    objHeader.srt.transl.x = createInfo->x;
    objHeader.srt.transl.y = createInfo->y;
    objHeader.srt.transl.z = createInfo->z;
    objHeader.createInfo = createInfo;
    objHeader.tabIdx = tabIdx;
    objHeader.id = objId;
    objHeader.unk0xb2 = param4;
    objHeader.mapID = mapID;
    objHeader.unk_0xa2 = -1;
    objHeader.unk0xb4 = -1;
    objHeader.srt.scale = def->scale;
    objHeader.unk_0x36 = 0xFF;
    objHeader.ptr0xcc = NULL;
    objHeader.unk0x3c = createInfo->unk6 << 3;
    objHeader.unk0x40 = createInfo->unk7 << 3;
    objHeader.dll = NULL;

    if (def->dllID != 0) {
        objHeader.dll = (DLLInst_Object*)dll_load(def->dllID, 6, 1);
    }

    flags = func_80022828(&objHeader);

    if (def->flags & 0x20) {
        flags &= ~1;
    } else {
        flags |= 1;
    }

    if (def->shadowType != 0) {
        flags |= 2;
    } else {
        flags &= ~2;
    }

    if (def->flags & 1) {
        flags |= 0x200;
    }

    var = obj_calc_mem_size(&objHeader, def, flags);

    obj = (Object*)malloc(var, ALLOC_TAG_OBJECTS_COL, NULL);

    if (obj == NULL) {
        recomp_eprintf("Failed to setup object %d. Malloc failed.\n", objId);
        obj_free_objdef(tabIdx);
        return NULL;
    }

    bcopy2(&objHeader, obj, sizeof(Object));
    bzero2((void*)((u32)obj + sizeof(Object)), var - sizeof(Object));

    modelCount = def->numModels;

    obj->modelInsts = (ModelInstance**)((u32)obj + sizeof(Object));

    modelLoadFailed = FALSE;
    var = 0;
    
    if (!(flags & 0x200)) {
        if (flags & 0x400) {
            var = (flags >> 11) & 0xf;

            if (var < modelCount) {
                obj->modelInsts[var] = model_load_create_instance(-def->pModelList[var], flags);

                if (obj->modelInsts[var] == NULL) {
                    modelLoadFailed = TRUE;
                    goto modelLoadFailedLabel;
                } else {
                    tempModel = obj->modelInsts[var];
                    func_80021E74(obj->srt.scale, tempModel);
                }
            }
        } else {
            for (; var < modelCount; var++) {
                obj->modelInsts[var] = model_load_create_instance(-def->pModelList[var], flags);
                if (obj->modelInsts[var] == NULL) {
                    modelLoadFailed = TRUE;
                } else {
                    tempModel = obj->modelInsts[var];
                    func_80021E74(obj->srt.scale, tempModel);
                }
            }
        }
    }

    modelLoadFailedLabel:
    if (modelLoadFailed) {
        recomp_eprintf("Failed to setup object %d. Failed to load models.\n", objId);
        func_80022200(obj, modelCount, objId);
        obj_free_objdef(tabIdx);
        return NULL;
    }
     
    addr = obj_alloc_object_state(obj, (u32)&obj->modelInsts[def->numModels]);

    if (flags & 0x40) {
        addr = func_80022868(obj->id, obj, addr);
    }

    if (flags & 0x100) {
        addr = func_8002298C(obj->id, obj->modelInsts[0], obj, addr);
    }

    if ((flags & 0x2) && (def->shadowType != 0)) {
        addr = func_8004D8A4(obj, addr, 0);
    }

    obj->unk_0xa8 = func_80022150(obj) * obj->srt.scale;

    if (def->unk8F != 0) {
        addr = func_8002667C(obj, addr);

        if (def->unk93 & 0x8) {
            addr = func_80026BD8(obj, addr);
        }
    }

    if (def->unk72 != 0) {
        obj->ptr0x6c = (u16*)align_4(addr);
        addr = (u32)obj->ptr0x6c + (def->unk72 * 0x12);
    }

    if (def->numTextures != 0) {
        obj->ptr0x70 = (void*)align_4(addr);
        addr = (u32)obj->ptr0x70 + (def->numTextures * 0x10);
    }

    if (def->unk9b != 0) {
        obj->unk0x74 = align_4(addr);
        addr = (u32)obj->unk0x74 + (def->unk9b * 0x18);
    }

    if (def->unk8F != 0 && def->unk74 != 0) {
        addr = align_4(addr);
        addr = func_80026A20(obj->id, obj->modelInsts[0], obj->objhitInfo, addr, obj);
    }

    if (def->unk9b != 0) {
        obj->unk_0x78 = (ObjectStruct78*)align_4(addr);

        for (j = 0; j < def->unk9b; j++) {
            obj->unk_0x78[j].unk4 = def->unk40[j].unk10;
            obj->unk_0x78[j].unk0 = def->unk40[j].unk0c;
            obj->unk_0x78[j].unk3 = def->unk40[j].unk0f;
            obj->unk_0x78[j].unk1 = def->unk40[j].unk0d;
            obj->unk_0x78[j].unk2 = def->unk40[j].unk0e;
        }
    }

    obj->parent = parent;
    
    return obj;
}

RECOMP_PATCH Object *obj_create(ObjCreateInfo *createInfo, u32 createFlags, s32 mapID, s32 param4, Object *parent) {
    Object *obj;

    obj = NULL;
    queue_load_map_object(&obj, createInfo, createFlags, mapID, param4, parent, 0);
    if (obj == NULL) {
        recomp_eprintf("Failed to load object: %d\n", createInfo->objId);
        return NULL;
    }
    obj_add_object(obj, createFlags);

    return obj;
}
