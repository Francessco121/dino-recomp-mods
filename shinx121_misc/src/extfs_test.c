#include "PR/ultratypes.h"
#include "game/objects/object_id.h"
#include "game/objects/object.h"
#include "recomputils.h"
#include "sys/fs.h"
#include "sys/map.h"
#include "sys/map_enums.h"

#include "extfs.h"

#include "mod_common.h"

INCBIN(krystalModelPatch, "assets/models0 - Krystal eye fix.bin");
INCBIN(villagePatch, "assets/blocks0643 - LightFoot Village LOD.bin");

EXTFS_ON_LOAD_REPLACEMENTS_CALLBACK void my_extfs_replacements() {
    extfs_tab_set_entry_replacement(MODELS_TAB, 0, krystalModelPatch, krystalModelPatch_end - krystalModelPatch);
    extfs_tab_set_entry_replacement(BLOCKS_TAB, 643, villagePatch, villagePatch_end - villagePatch);
}

static void cc_log_patch() {
    MapHeader *header = extfs_tab_get_entry(MAPS_TAB, MAP_CAPE_CLAW * 7, NULL);
    void *objects = extfs_tab_get_entry(MAPS_TAB, MAP_CAPE_CLAW * 7 + 4, NULL);

    ObjCreateInfo *objInfo = (ObjCreateInfo*)objects;

    for (s32 i = 0; i < header->objectInstanceCount; i++) {
        if (objInfo->uID == 11480) {
            // Swap DFlog with BWLog
            objInfo->objId = OBJ_BWLog;
        }

        objInfo = (ObjCreateInfo*)((u32)objInfo + (objInfo->quarterSize << 2));
    }
}

EXTFS_ON_LOAD_MODIFICATIONS_CALLBACK void my_extfs_modifications() {
    ObjDef *sabreObjDef = extfs_tab_get_entry(OBJECTS_TAB, 1, NULL);
    u32 *sabreModelList = (u32*)((u32)sabreObjDef + (u32)sabreObjDef->pModelList);
    sabreModelList[0] = 9;
    sabreModelList[1] = 9;

    cc_log_patch();
}
