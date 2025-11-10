#include "dll27_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "object_debug.h"
#include "../3d.h"

#include "game/objects/object.h"
#include "game/objects/object_id.h"
#include "dlls/engine/27.h"

static DLL27_Data *get_dll27_data(Object *obj) {
    switch (obj->def->dllID) {
        case 0x8001: // Player
        case 0x81ed: // KT_Rex
            return (DLL27_Data*)((u32)obj->data + 0x4);
        case 0x811f: // Snowhorn
            return (DLL27_Data*)((u32)obj->data + 0x170);
        case 0x81f6: // IMSnowBike
            return (DLL27_Data*)((u32)obj->data + 0x4C);
        default: // Those that have DLL27 data at the start of their object data
            return (DLL27_Data*)obj->data;
    }
}

static void draw_bounds(Object *obj, DLL27_Data *data, u32 color) {
    s32 count1 = data->numTestPoints >> 4;
    s32 count2 = data->numTestPoints & 0xF;

    for (s32 i = 0; i < count1; i++) {
        draw_3d_text(data->unk8[i].x, data->unk8[i].y, data->unk8[i].z, recomp_sprintf_helper("unk8[%d]", i), 0xFF00FFFF);
    }

    for (s32 i = 0; i < count2; i++) {
        draw_3d_text(data->unkE0[i].x, data->unkE0[i].y, data->unkE0[i].z, recomp_sprintf_helper("unkE0[%d]", i), 0xFF00FFFF);
    }

    f32 tx = obj->positionMirror.x + (data->floorNormalX * 5);
    f32 ty = obj->positionMirror.y + (data->floorNormalY * 5);
    f32 tz = obj->positionMirror.z + (data->floorNormalZ * 5);

    draw_3d_line(
        obj->positionMirror.x, obj->positionMirror.y, obj->positionMirror.z, 
        tx, ty, tz, 
        color);
}

static s32 drawBounds = FALSE;

void dll27_debug_tab(Object *obj) {
    DLL27_Data *data = get_dll27_data(obj);
    if (data == NULL) {
        return;
    }

    dbgui_checkbox("Draw", &drawBounds);

    s32 count1 = data->numTestPoints >> 4;
    s32 count2 = data->numTestPoints & 0xF;

    dbgui_textf("Terrain test points: (numTestPoints >> 4): %d", count1);
    dbgui_textf("Hits test points (numTestPoints & 0xF): %d", count2);

    dbgui_textf("flags: 0x%X", data->flags);
    if (data->unk4 != NULL) {
        if (dbgui_tree_node("unk4")) {
            for (s32 i = 0; i < count1; i++) {
                dbgui_textf("[%d] %f,%f,%f", i, data->unk4[i].x, data->unk4[i].y, data->unk4[i].z);
            }
            
            dbgui_tree_pop();
        }
    } else {
        dbgui_text("unk4: (null)");
    }
    if (dbgui_tree_node("unk8")) {
        for (s32 i = 0; i < count1; i++) {
            dbgui_textf("[%d] %f,%f,%f", i, data->unk8[i].x, data->unk8[i].y, data->unk8[i].z);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("unk38")) {
        for (s32 i = 0; i < count1; i++) {
            dbgui_textf("[%d] %f,%f,%f", i, data->unk38[i].x, data->unk38[i].y, data->unk38[i].z);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("unk68")) {
        Unk80027934 *unk68 = &data->unk68;

        if (dbgui_tree_node("unk40")) {
            for (s32 i = 0; i < count1; i++) {
                dbgui_textf("[%d] %f", i, unk68->unk40[i]);
            }
            dbgui_tree_pop();
        }
        if (dbgui_tree_node("unk50")) {
            for (s32 i = 0; i < count1; i++) {
                dbgui_textf("[%d] %d", i, unk68->unk50[i]);
            }
            dbgui_tree_pop();
        }
        if (dbgui_tree_node("unk54")) {
            for (s32 i = 0; i < count1; i++) {
                dbgui_textf("[%d] %d", i, unk68->unk54[i]);
            }
            dbgui_tree_pop();
        }
        if (dbgui_tree_node("unk58")) {
            for (s32 i = 0; i < count1; i++) {
                dbgui_textf("[%d] %d", i, unk68->unk58[i]);
            }
            dbgui_tree_pop();
        }
        dbgui_textf("unk68: 0x%X", data->unk68);
        
        dbgui_tree_pop();
    }
    if (data->unkD4 != NULL) {
        if (dbgui_tree_node("unkD4")) {
            object_edit_contents(data->unkD4);
            dbgui_tree_pop();
        }
    } else {
        dbgui_text("unkD4: (null)");
    }
    if (data->localHitsTestPoints != NULL) {
        if (dbgui_tree_node("localHitsTestPoints")) {
            for (s32 i = 0; i < count2; i++) {
                dbgui_textf("[%d] %f,%f,%f", i, data->localHitsTestPoints[i].x, data->localHitsTestPoints[i].y, data->localHitsTestPoints[i].z);
            }
            
            dbgui_tree_pop();
        }
    } else {
        dbgui_text("localHitsTestPoints: (null)");
    }
    if (data->hitsTestRadii != NULL) {
        if (dbgui_tree_node("hitsTestRadii")) {
            for (s32 i = 0; i < count2; i++) {
                dbgui_textf("[%d] %f", i, data->hitsTestRadii[i]);
            }
            
            dbgui_tree_pop();
        }
    } else {
        dbgui_text("hitsTestRadii: (null)");
    }
    if (dbgui_tree_node("unkE0")) {
        for (s32 i = 0; i < count2; i++) {
            dbgui_textf("[%d] %f,%f,%f", i, data->unkE0[i].x, data->unkE0[i].y, data->unkE0[i].z);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("unk110")) {
        for (s32 i = 0; i < count2; i++) {
            dbgui_textf("[%d] %f,%f,%f", i, data->unk110[i].x, data->unk110[i].y, data->unk110[i].z);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("unk140")) {
        Func_80059C40_Struct *unk140 = &data->unk140;

        if (unk140->unk0 != NULL) {
            if (dbgui_tree_node("unk0")) {
                object_edit_contents(unk140->unk0);
                dbgui_tree_pop();
            }
        } else {
            dbgui_text("unk0: (null)");
        }
        
        dbgui_textf("unk4: %f", unk140->unk4);
        dbgui_textf("unk8: %f", unk140->unk8);
        dbgui_textf("unkC: %f", unk140->unkC);
        dbgui_textf("unk10: %f", unk140->unk10);
        dbgui_textf("unk14: %f", unk140->unk14);
        dbgui_textf("unk18: %f", unk140->unk18);
        dbgui_textf("unk1C: %f,%f,%f,%f", unk140->unk1C.x, unk140->unk1C.y, unk140->unk1C.z, unk140->unk1C.w);
        dbgui_textf("unk38: %f,%f,%f", unk140->unk38.x, unk140->unk38.y, unk140->unk38.z);
        dbgui_textf("unk44: %f", unk140->unk44);
        dbgui_textf("unk48: %f", unk140->unk48);
        dbgui_textf("unk50: %d", unk140->unk50);
        dbgui_textf("unk52: %d", unk140->unk52);
        
        dbgui_tree_pop();
    }
    dbgui_textf("relativeFloorPitchSmooth: %d", data->relativeFloorPitchSmooth);
    dbgui_textf("relativeFloorRollSmooth: %d", data->relativeFloorRollSmooth);
    dbgui_textf("relativeFloorPitch: %d", data->relativeFloorPitch);
    dbgui_textf("relativeFloorRoll: %d", data->relativeFloorRoll);
    dbgui_textf("floorNormalX: %f", data->floorNormalX);
    dbgui_textf("floorNormalY: %f", data->floorNormalY);
    dbgui_textf("floorNormalZ: %f", data->floorNormalZ);
    dbgui_textf("floorDist: %f", data->floorDist);
    dbgui_textf("unk1AC: %f", data->unk1AC);
    dbgui_textf("underwaterDist: %f", data->underwaterDist);
    dbgui_textf("floorY: %f", data->floorY);
    dbgui_textf("waterY: %f", data->waterY);
    if (dbgui_tree_node("floorDistList")) {
        for (s32 i = 0; i < count1; i++) {
            dbgui_textf("[%d] %f", i, data->floorDistList[i]);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("unk1CC")) {
        for (s32 i = 0; i < count1; i++) {
            dbgui_textf("[%d] %f", i, data->unk1CC[i]);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("underwaterDistList")) {
        for (s32 i = 0; i < count1; i++) {
            dbgui_textf("[%d] %f", i, data->underwaterDistList[i]);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("floorYList")) {
        for (s32 i = 0; i < count1; i++) {
            dbgui_textf("[%d] %f", i, data->floorYList[i]);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("waterYList")) {
        for (s32 i = 0; i < count1; i++) {
            dbgui_textf("[%d] %f", i, data->waterYList[i]);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("waterNormalXList")) {
        for (s32 i = 0; i < count1; i++) {
            dbgui_textf("[%d] %f", i, data->waterNormalXList[i]);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("waterNormalYList")) {
        for (s32 i = 0; i < count1; i++) {
            dbgui_textf("[%d] %f", i, data->waterNormalYList[i]);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("waterNormalZList")) {
        for (s32 i = 0; i < count1; i++) {
            dbgui_textf("[%d] %f", i, data->waterNormalZList[i]);
        }
        
        dbgui_tree_pop();
    }
    if (dbgui_tree_node("aabb")) {
        dbgui_textf("min: %d,%d,%d", data->aabb.min.x, data->aabb.min.y, data->aabb.min.z);
        dbgui_textf("max: %d,%d,%d", data->aabb.max.x, data->aabb.max.y, data->aabb.max.z);
        
        dbgui_tree_pop();
    }
    dbgui_textf("boundsYExtension: %d", data->boundsYExtension);
    dbgui_textf("mode: %d", data->mode);
    dbgui_textf("numTestPoints: %d", data->numTestPoints);
    dbgui_textf("unk259: %d", data->unk259);
    dbgui_textf("hitsTouchBits: %d", data->hitsTouchBits);
    dbgui_textf("unk25B: %d", data->unk25B);
    dbgui_textf("unk25C: 0x%X", data->unk25C);
    dbgui_textf("unk25D: 0x%X", data->unk25D);

    if (drawBounds) {
        draw_bounds(obj, data, 0xFFFF00FF);
    }
}