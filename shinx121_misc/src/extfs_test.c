#include "PR/ultratypes.h"
#include "game/objects/object_id.h"
#include "game/objects/object.h"
#include "recomputils.h"
#include "sys/fs.h"
#include "sys/map.h"

#include "extfs.h"

#include "mod_common.h"

INCBIN(krystalModelPatch, "assets/models0 - Krystal eye fix.bin");
INCBIN(villagePatch, "assets/blocks0643 - LightFoot Village LOD.bin");

EXTFS_ON_LOAD_REPLACEMENTS_CALLBACK void my_extfs_replacements() {
    extfs_tab_set_entry_replacement(MODELS_TAB, 0, krystalModelPatch, krystalModelPatch_end - krystalModelPatch);
    extfs_tab_set_entry_replacement(BLOCKS_TAB, 643, villagePatch, villagePatch_end - villagePatch);
}

EXTFS_ON_LOAD_MODIFICATIONS_CALLBACK void my_extfs_modifications() {
    ObjDef *sabreObjDef = extfs_tab_get_entry(OBJECTS_TAB, 1, NULL);
    u32 *sabreModelList = (u32*)((u32)sabreObjDef + (u32)sabreObjDef->pModelList);
    sabreModelList[0] = 9;
    sabreModelList[1] = 9;

    // u32 capeClawObjects = (29 * 7) + 4;
    // u32 capeClawObjectsSize;
    // ObjCreateInfo *capeClawObjInfos = extfs_tab_get_entry(MAPS_TAB, capeClawObjects, &capeClawObjectsSize);

    // MapHeader *header = extfs_tab_get_entry(MAPS_TAB, 29 * 7, NULL);

    // u8 extradata[] = {
    //     0x0, 0x3 ,0x49 ,0xf3 ,0xff ,0xff ,0xff ,0xff ,0xff ,0xff ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 ,0x0 
    //     ,0x0 ,0x3 ,0x0 ,0xe ,0x0 ,0x2 ,0x0 ,0x0 ,0xff ,0x1 ,0xff ,0xff ,0xe ,0x0 ,0x0 ,0x0 ,0x0 ,0x0};

    // ObjCreateInfo *obj = extfs_tab_resize_entry(MAPS_TAB, capeClawObjects, 0x50 * 1);
    // u32 start = (u32)obj;

    // for (s32 i = 0; i < 1; i++) {
    //     obj->objId = OBJ_BossDrakor;
    //     obj->quarterSize = 0x50 / 4;
    //     obj->setup = 0;
    //     obj->loadParamA = 1;
    //     obj->loadParamB = 4;
    //     obj->loadDistance = 0x80;
    //     obj->fadeDistance = 0x64;
    //     obj->x = 347.064453f + (3 * i);
    //     obj->y = 5.390846f + (60 * i);
    //     obj->z = 959.935547f + (3 * i);
    //     obj->uID = i;

    //     // u8 *d = (u8*)((u32)obj + sizeof(ObjCreateInfo));
    //     // for (s32 k = 0; k < 36; k++) {
    //     //     d[k] = extradata[k];
    //     // }

    //     obj = (ObjCreateInfo*)((u32)obj + (obj->quarterSize << 2));
    // }


    // header->objectInstanceCount = 1;
    // header->objectInstancesFileLength = 0x50 * 1;
}
