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
        if (dbgui_begin_tab_item("Create Info", NULL)) {
            if (obj->createInfo != NULL) {
                TriggerCreateInfo *createInfo = (TriggerCreateInfo*)obj->createInfo;
                dbgui_textf("objId: %d", createInfo->base.objId);
                dbgui_textf("quarterSize: %d", createInfo->base.quarterSize);
                dbgui_textf("setup: %d", createInfo->base.setup);
                dbgui_textf("loadParamA: %d", createInfo->base.loadParamA);
                dbgui_textf("loadParamB: %d", createInfo->base.loadParamB);
                dbgui_textf("loadDistance: %d", createInfo->base.loadDistance);
                dbgui_textf("fadeDistance: %d", createInfo->base.fadeDistance);
                dbgui_textf("x: %f", createInfo->base.x);
                dbgui_textf("y: %f", createInfo->base.y);
                dbgui_textf("z: %f", createInfo->base.z);
                dbgui_textf("uID: %d", createInfo->base.uID);

                dbgui_text("commands:");
                for (s32 i = 0; i < 8; i++) {
                    static DbgUiInputIntOptions inputOptions = {
                        .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal | DBGUI_INPUT_TEXT_FLAGS_AlwaysOverwrite,
                        .step = 0,
                        .stepFast = 0
                    };
                    dbgui_textf("[%d]: %02X %02X %02X %02X", i,
                        createInfo->commands[i].condition, createInfo->commands[i].id, 
                        createInfo->commands[i].param1, createInfo->commands[i].param2);
                    s32 word = *((s32*)&createInfo->commands[i]);
                    if (dbgui_input_int_ext(recomp_sprintf_helper("##commands[%d]", i), &word, &inputOptions)) {
                        *((s32*)&createInfo->commands[i]) = word;
                    }
                }

                dbgui_textf("localID: %d", createInfo->localID);
                dbgui_textf("sizeX: %d", createInfo->sizeX);
                dbgui_textf("sizeY: %d", createInfo->sizeY);
                dbgui_textf("sizeZ: %d", createInfo->sizeZ);
                dbgui_textf("rotationY: %d", createInfo->rotationY);
                dbgui_textf("rotationX: %d", createInfo->rotationX);
                dbgui_textf("activatorObjType: %d", createInfo->activatorObjType);
                dbgui_textf("bitFlagID: %x", createInfo->bitFlagID);
                dbgui_textf("timerDuration: %d", createInfo->timerDuration);
                dbgui_textf("conditionBitFlagIDs: [%x, %x, %x, %x]", 
                    createInfo->conditionBitFlagIDs[0], createInfo->conditionBitFlagIDs[1],
                    createInfo->conditionBitFlagIDs[2], createInfo->conditionBitFlagIDs[3]);
            }

            dbgui_end_tab_item();
        }

        if (dbgui_begin_tab_item("State", NULL)) {
            if (obj->state != NULL) {
                TriggerState *state = (TriggerState*)obj->state;

                dbgui_textf("flags: %x", state->flags);
                dbgui_textf("radiusSquared: %f", state->radiusSquared);
                dbgui_textf("elapsedTicks: %d", state->elapsedTicks);
                dbgui_textf("lookVector: %f,%f,%f", state->lookVector.x, state->lookVector.y, state->lookVector.z);
                dbgui_textf("lookVectorNegDot: %f", state->lookVectorNegDot);
                dbgui_textf("activatorPrevPos: %f,%f,%f", state->activatorPrevPos.x, state->activatorPrevPos.y, state->activatorPrevPos.z);
                dbgui_textf("activatorCurrPos: %f,%f,%f", state->activatorCurrPos.x, state->activatorCurrPos.y, state->activatorCurrPos.z);
                dbgui_textf("planeMin: %f,%f,%f", state->planeMin.x, state->planeMin.y, state->planeMin.z);
                dbgui_textf("planeMax: %f,%f,%f", state->planeMax.x, state->planeMax.y, state->planeMax.z);
                dbgui_textf("bitFlagID: %x", state->bitFlagID);
                dbgui_textf("conditionBitFlagIDs: [%x, %x, %x, %x]", 
                    state->conditionBitFlagIDs[0], state->conditionBitFlagIDs[1], 
                    state->conditionBitFlagIDs[2], state->conditionBitFlagIDs[3]);
                dbgui_textf("soundHandles: [%x, %x, %x, %x, %x, %x, %x, %x]", 
                    state->soundHandles[0], state->soundHandles[1], state->soundHandles[2], state->soundHandles[3],
                    state->soundHandles[4], state->soundHandles[5], state->soundHandles[6], state->soundHandles[7]);
                dbgui_textf("scripts: [%p, %p, %p, %p, %p, %p, %p, %p]", 
                    state->scripts[0], state->scripts[1], state->scripts[2], state->scripts[3],
                    state->scripts[4], state->scripts[5], state->scripts[6], state->scripts[7]);
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
        TriggerCreateInfo *createInfo = (TriggerCreateInfo*)obj->createInfo;
        if (createInfo != NULL) {
            draw_3d_sphere(
                position.x,
                position.y,
                position.z,
                (f32)(createInfo->sizeX << 1),
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
