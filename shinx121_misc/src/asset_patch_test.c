#include "imports.h"
#include "modding.h"
#include "mod_fs.h"

#include "mod_common.h"

INCBIN(krystalModelPatch, "assets/models0 - Krystal eye fix.bin");
INCBIN(villagePatch, "assets/blocks0643 - LightFoot Village LOD.bin");

MODFS_ON_INIT_CALLBACK void my_mod_fs_init_callback() {
    register_model_replacement(0, krystalModelPatch, krystalModelPatch_end - krystalModelPatch);
    register_block_replacement(643, villagePatch, villagePatch_end - villagePatch);
}