#include "cfguardian_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "dlls/engine/27.h"
#include "dlls/engine/53_movelib.h"
#include "dlls/objects/common/collectable.h"
#include "game/gamebits.h"
#include "game/objects/interaction_arrow.h"
#include "game/objects/object_id.h"
#include "sys/main.h"
#include "sys/objects.h"
#include "sys/objprint.h"
#include "sys/objtype.h"
#include "sys/objmsg.h"
#include "sys/print.h"
#include "sys/joypad.h"
#include "sys/rand.h"
#include "dll.h"
#include "macros.h"

typedef struct {
/*000*/ MoveLibData movedata;
/*4B8*/ HeadAnimation unk4B8;
/*4DC*/ HeadAnimation unk4DC;
/*500*/ u32 unk500; // unused sound handle
/*504*/ u8 _unk504[0x50C - 0x504];
/*50C*/ Object* dustObjs[6];
/*524*/ Collectable_Setup* dustSetups[6];
/*53C*/ UnkCurvesStruct unk53C;
/*644*/ u8 _unk644[0x67C - 0x644];
/*67C*/ f32 animRate;
/*680*/ DLL27_Data collider;
/*8E0*/ SRT walkTarget;
/*8F8*/ u8 state;
/*8F9*/ u8 _unk8F9[0x908 - 0x8F9];
/*908*/ s32 unk908;
/*90C*/ s32 unk90C;
/*910*/ u8 unk910;
/*911*/ s8 unk911;
/*912*/ u8 unk912;
} CFGuardian_Data;

void cfguardian_debug_tab(Object *obj) {
    CFGuardian_Data *objdata = obj->data;

    dbgui_textf("animRate: %f", objdata->animRate);
    dbgui_textf("walkTarget: %f, %f, %f; %d", 
        objdata->walkTarget.transl.x, objdata->walkTarget.transl.y, objdata->walkTarget.transl.z, 
        objdata->walkTarget.yaw);
    dbgui_textf("state: %d", objdata->state);
    dbgui_textf("unk908: %d", objdata->unk908);
    dbgui_textf("unk90C: %d", objdata->unk90C);
    dbgui_textf("unk910: %d", objdata->unk910);
    dbgui_textf("unk911: %d", objdata->unk911);
    dbgui_textf("unk912: %d", objdata->unk912);
}
