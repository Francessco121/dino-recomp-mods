#include "trigger_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "../3d.h"

#include "game/objects/object.h"
#include "game/objects/object_id.h"
#include "recomputils.h"
#include "sys/gfx/model.h"
#include "sys/math.h"

#include "trigger.h"

#include "recomp/dlls/objects/325_trigger_recomp.h"

extern ModelInstance *sPlaneModel;

void trigger_debug_tab(Object *obj) {
    if (dbgui_begin_tab_bar("##edit")) {
        if (dbgui_begin_tab_item("Object Setup", NULL)) {
            if (obj->setup != NULL) {
                Trigger_Setup *objsetup = (Trigger_Setup*)obj->setup;
                dbgui_textf("objId: %d", objsetup->base.objId);
                dbgui_textf("quarterSize: %d", objsetup->base.quarterSize);
                dbgui_textf("setup: %d", objsetup->base.setup);
                dbgui_textf("loadParamA: %d", objsetup->base.loadParamA);
                dbgui_textf("loadParamB: %d", objsetup->base.loadParamB);
                dbgui_textf("loadDistance: %d", objsetup->base.loadDistance);
                dbgui_textf("fadeDistance: %d", objsetup->base.fadeDistance);
                dbgui_textf("x: %f", objsetup->base.x);
                dbgui_textf("y: %f", objsetup->base.y);
                dbgui_textf("z: %f", objsetup->base.z);
                dbgui_textf("uID: %d", objsetup->base.uID);

                dbgui_text("commands:");
                for (s32 i = 0; i < 8; i++) {
                    static DbgUiInputIntOptions inputOptions = {
                        .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal | DBGUI_INPUT_TEXT_FLAGS_AlwaysOverwrite,
                        .step = 0,
                        .stepFast = 0
                    };
                    dbgui_textf("[%d]: %02X %02X %02X %02X", i,
                        objsetup->commands[i].condition, objsetup->commands[i].id, 
                        objsetup->commands[i].param1, objsetup->commands[i].param2);
                    s32 word = *((s32*)&objsetup->commands[i]);
                    if (dbgui_input_int_ext(recomp_sprintf_helper("##commands[%d]", i), &word, &inputOptions)) {
                        *((s32*)&objsetup->commands[i]) = word;
                    }
                }

                dbgui_textf("localID: %d", objsetup->localID);
                dbgui_textf("sizeX: %d", objsetup->sizeX);
                dbgui_textf("sizeY: %d", objsetup->sizeY);
                dbgui_textf("sizeZ: %d", objsetup->sizeZ);
                dbgui_textf("rotationY: %d", objsetup->rotationY);
                dbgui_textf("rotationX: %d", objsetup->rotationX);
                dbgui_textf("activatorObjType: %d", objsetup->activatorObjType);
                dbgui_textf("bitFlagID: %x", objsetup->bitFlagID);
                dbgui_textf("timerDuration: %d", objsetup->timerDuration);
                dbgui_textf("conditionBitFlagIDs: [%x, %x, %x, %x]", 
                    objsetup->conditionBitFlagIDs[0], objsetup->conditionBitFlagIDs[1],
                    objsetup->conditionBitFlagIDs[2], objsetup->conditionBitFlagIDs[3]);
            }

            dbgui_end_tab_item();
        }

        if (dbgui_begin_tab_item("Object Data", NULL)) {
            if (obj->setup != NULL) {
                Trigger_Data *objdata = (Trigger_Data*)obj->data;

                dbgui_textf("flags: %x", objdata->flags);
                dbgui_textf("radiusSquared: %f", objdata->radiusSquared);
                dbgui_textf("elapsedTicks: %d", objdata->elapsedTicks);
                dbgui_textf("lookVector: %f,%f,%f", objdata->lookVector.x, objdata->lookVector.y, objdata->lookVector.z);
                dbgui_textf("lookVectorNegDot: %f", objdata->lookVectorNegDot);
                dbgui_textf("activatorPrevPos: %f,%f,%f", objdata->activatorPrevPos.x, objdata->activatorPrevPos.y, objdata->activatorPrevPos.z);
                dbgui_textf("activatorCurrPos: %f,%f,%f", objdata->activatorCurrPos.x, objdata->activatorCurrPos.y, objdata->activatorCurrPos.z);
                dbgui_textf("planeMin: %f,%f,%f", objdata->planeMin.x, objdata->planeMin.y, objdata->planeMin.z);
                dbgui_textf("planeMax: %f,%f,%f", objdata->planeMax.x, objdata->planeMax.y, objdata->planeMax.z);
                dbgui_textf("bitFlagID: %x", objdata->bitFlagID);
                dbgui_textf("conditionBitFlagIDs: [%x, %x, %x, %x]", 
                    objdata->conditionBitFlagIDs[0], objdata->conditionBitFlagIDs[1], 
                    objdata->conditionBitFlagIDs[2], objdata->conditionBitFlagIDs[3]);
                dbgui_textf("soundHandles: [%x, %x, %x, %x, %x, %x, %x, %x]", 
                    objdata->soundHandles[0], objdata->soundHandles[1], objdata->soundHandles[2], objdata->soundHandles[3],
                    objdata->soundHandles[4], objdata->soundHandles[5], objdata->soundHandles[6], objdata->soundHandles[7]);
                dbgui_textf("scripts: [%p, %p, %p, %p, %p, %p, %p, %p]", 
                    objdata->scripts[0], objdata->scripts[1], objdata->scripts[2], objdata->scripts[3],
                    objdata->scripts[4], objdata->scripts[5], objdata->scripts[6], objdata->scripts[7]);
            }

            dbgui_end_tab_item();
        }
        
        dbgui_end_tab_bar();
    }
}

static void draw_plane_line(MtxF *mtx, f32 x1, f32 y1, f32 z1, f32 x2, f32 y2, f32 z2, u32 color) {
    vec3_transform(mtx, x1, y1, z1, &x1, &y1, &z1);
    vec3_transform(mtx, x2, y2, z2, &x2, &y2, &z2);

    draw_3d_line(x1, y1, z1, x2, y2, z2, color);
}

static void draw_trigger_plane(Object *obj, u32 color) {
    MtxF mtx;
    matrix_from_srt(&mtx, &obj->srt);

    Vtx *vertex = &sPlaneModel->model->vertices[1];
    f32 minX, minY, minZ;
    f32 maxX, maxY, maxZ;
    minX = maxX = (f32)vertex->v.ob[0];
    minY = maxY = (f32)vertex->v.ob[1];
    minZ = maxZ = (f32)vertex->v.ob[2];

    for (s32 i = 1; i < sPlaneModel->model->vertexCount; i++, vertex++) {
        if (vertex->v.ob[0] < minX) {
            minX = vertex->v.ob[0];
        }
        if (vertex->v.ob[0] > maxX) {
            maxX = vertex->v.ob[0];
        }

        if (vertex->v.ob[1] < minY) {
            minY = vertex->v.ob[1];
        }
        if (vertex->v.ob[1] > maxY) {
            maxY = vertex->v.ob[1];
        }

        if (vertex->v.ob[2] < minZ) {
            minZ = vertex->v.ob[2];
        }
        if (vertex->v.ob[2] > maxZ) {
            maxZ = vertex->v.ob[2];
        }
    }

    draw_plane_line(&mtx, minX, minY, minZ, maxX, minY, minZ, color);
    draw_plane_line(&mtx, minX, maxY, minZ, maxX, maxY, minZ, color);

    draw_plane_line(&mtx, minX, minY, minZ, minX, maxY, minZ, color);
    draw_plane_line(&mtx, maxX, minY, minZ, maxX, maxY, minZ, color);

    draw_plane_line(&mtx, minX, minY, maxZ, maxX, minY, maxZ, color);
    draw_plane_line(&mtx, minX, maxY, maxZ, maxX, maxY, maxZ, color);

    draw_plane_line(&mtx, minX, minY, maxZ, minX, maxY, maxZ, color);
    draw_plane_line(&mtx, maxX, minY, maxZ, maxX, maxY, maxZ, color);
}

void draw_trigger(Object *obj, u32 color) {
    Vec3f position = obj->positionMirror3;

    switch (obj->id) {
    case OBJ_TriggerPoint: {
        Trigger_Setup *setup = (Trigger_Setup*)obj->setup;
        if (setup != NULL) {
            draw_3d_sphere(
                position.x,
                position.y,
                position.z,
                (f32)(setup->sizeX << 1),
                color
            );
        }
        break;
    }
    case OBJ_TriggerPlane:
        draw_trigger_plane(obj, color);
        break;
    default:
        draw_3d_sphere(
            position.x,
            position.y,
            position.z,
            2 * obj->srt.scale,
            color
        );
        break;
    }
}
