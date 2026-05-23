#include "trigger_debug.h"

#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"
#include "../3d.h"
#include "../debug_common.h"

#include "game/objects/object.h"
#include "game/objects/object_id.h"
#include "dlls/objects/325_trigger.h"
#include "sys/gfx/model.h"
#include "sys/math.h"

#include "recomp/dlls/objects/325_trigger_recomp.h"

extern ModelInstance *sPlaneModel;

static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

const char* get_trigger_name(s32 id) {
    switch (id) {
        case TRG_CMD_HAZARD:
            return "HAZARD";
        case TRG_CMD_2:
            return "2";
        case TRG_CMD_MUSIC_ACTION:
            return "MUSIC_ACTION";
        case TRG_CMD_SOUND:
            return "SOUND";
        case TRG_CMD_5:
            return "5";
        case TRG_CMD_CAMERA_ACTION:
            return "CAMERA_ACTION";
        case TRG_CMD_7:
            return "7";
        case TRG_CMD_TRACK:
            return "TRACK";
        case TRG_CMD_9:
            return "9";
        case TRG_CMD_ENV_FX:
            return "ENV_FX";
        case TRG_CMD_ANIM_SEQ:
            return "ANIM_SEQ";
        case TRG_CMD_TRIGGER:
            return "TRIGGER";
        case TRG_CMD_LIGHTING:
            return "LIGHTING";
        case TRG_CMD_STORYBOARD:
            return "STORYBOARD";
        case TRG_CMD_SETUP_POINT:
            return "SETUP_POINT";
        case TRG_CMD_LOD_MODEL:
            return "LOD_MODEL";
        case TRG_CMD_TRICKY_TALK_SEQ:
            return "TRICKY_TALK_SEQ";
        case TRG_CMD_BITS:
            return "BITS";
        case TRG_CMD_ENABLE_OBJ_GROUP:
            return "ENABLE_OBJ_GROUP";
        case TRG_CMD_DISABLE_OBJ_GROUP:
            return "DISABLE_OBJ_GROUP";
        case TRG_CMD_TEXTURE_LOAD:
            return "TEXTURE_LOAD";
        case TRG_CMD_TEXTURE_FREE:
            return "TEXTURE_FREE";
        case TRG_CMD_17:
            return "17";
        case TRG_CMD_SET_ACT:
            return "SET_MAP_ACT";
        case TRG_CMD_SCRIPT:
            return "SCRIPT";
        case TRG_CMD_WORLD_ENABLE_OBJ_GROUP:
            return "WORLD_ENABLE_OBJ_GROUP";
        case TRG_CMD_WORLD_DISABLE_OBJ_GROUP:
            return "WORLD_DISABLE_OBJ_GROUP";
        case TRG_CMD_KYTE_FLIGHT_GROUP:
            return "KYTE_FLIGHT_GROUP";
        case TRG_CMD_KYTE_TALK_SEQ:
            return "KYTE_TALK_SEQ";
        case TRG_CMD_WORLD_SET_ACT:
            return "WORLD_SET_MAP_ACT";
        case TRG_CMD_SAVE_POINT:
            return "SAVE_POINT";
        case TRG_CMD_MAP_LAYER:
            return "MAP_LAYER";
        case TRG_CMD_BITS_TOGGLE:
            return "BITS_TOGGLE";
        case TRG_CMD_TOGGLE_OBJ_GROUP:
            return "TOGGLE_OBJ_GROUP";
        case TRG_CMD_RESTART:
            return "RESTART";
        case TRG_CMD_WATER_FALLS_FLAGS:
            return "WATER_FALLS_FLAGS";
        case TRG_CMD_WATER_FALLS_FLAGS2:
            return "WATER_FALLS_FLAGS2";
        case TRG_CMD_SIDEKICK:
            return "SIDEKICK";
        default:
            return "<unknown>";
    }
}

const char* get_trigger_cond_name(s32 cond) {
    static char buffer[100];
    s32 offset = 0;
    if (cond & CMD_COND_IN) {
        offset += recomp_sprintf(buffer + offset, "%s, ", "In");
    }
    if (cond & CMD_COND_OUT) {
        offset += recomp_sprintf(buffer + offset, "%s, ", "Out");
    }
    if (cond & CMD_COND_RE_ENTER) {
        offset += recomp_sprintf(buffer + offset, "%s, ", "Re-enter");
    }
    if (cond & CMD_COND_RE_EXIT) {
        offset += recomp_sprintf(buffer + offset, "%s, ", "Re-exit");
    }
    if (cond & CMD_COND_CONTINUOUS) {
        offset += recomp_sprintf(buffer + offset, "%s, ", "Continuous");
    }
    if (cond & CMD_COND_RESTORE) {
        offset += recomp_sprintf(buffer + offset, "%s, ", "Restore");
    }

    offset = MAX(0, offset - 2);
    buffer[offset] = '\0';

    return buffer;
}

const char* get_trigger_param(TriggerCommand *cmd) {
    static char buffer[100];
    s32 offset = 0;
    
    switch (cmd->id) {
        case TRG_CMD_CAMERA_ACTION:
        case TRG_CMD_TRACK:
        case TRG_CMD_ANIM_SEQ:
        case TRG_CMD_SETUP_POINT:
            offset += recomp_sprintf(buffer + offset, "0x%X, 0x%X", cmd->param1, cmd->param2);
            break;
        case TRG_CMD_LOD_MODEL:
        case TRG_CMD_RESTART:
        case TRG_CMD_MAP_LAYER:
        case TRG_CMD_SIDEKICK:
        case TRG_CMD_WATER_FALLS_FLAGS:
        case TRG_CMD_WATER_FALLS_FLAGS2:
            offset += recomp_sprintf(buffer + offset, "0x%X", cmd->param1);
            break;
        case TRG_CMD_SCRIPT:
        case TRG_CMD_SAVE_POINT:
            offset += recomp_sprintf(buffer + offset, "0x%X", cmd->param2);
            break;
        case TRG_CMD_WORLD_ENABLE_OBJ_GROUP:
        case TRG_CMD_WORLD_DISABLE_OBJ_GROUP:
        case TRG_CMD_WORLD_SET_ACT:
            offset += recomp_sprintf(buffer + offset, "0x%X, 0x%X", cmd->param2, cmd->param1);
            break;
        case TRG_CMD_BITS: {
            s32 param = cmd->param2 | (cmd->param1 << 8);
            s32 entry;
            u32 value;

            entry = param & 0x3FFF;
            param >>= 14;
            offset += recomp_sprintf(buffer + offset, "Bit 0x%X", entry);

            if (param == 0) {
                offset += recomp_sprintf(buffer + offset, ", 0");
            } else if (param == 1) {
                offset += recomp_sprintf(buffer + offset, ", -1");
            } else if (param == 2) {
                offset += recomp_sprintf(buffer + offset, ", ~");
            }
            break;
        }
        case TRG_CMD_BITS_TOGGLE: {
            s32 param = cmd->param2 | (cmd->param1 << 8);
            s32 entry;
            u32 value;

            entry = param & 0x1FFF;
            param >>= 13;
            offset += recomp_sprintf(buffer + offset, "Bit 0x%X, Flip %d", entry, param);
            break;
        }
        default:
            offset += recomp_sprintf(buffer + offset, "0x%X", cmd->param2 | (cmd->param1 << 8));
            break;
    }

    if (offset >= (s32)sizeof(buffer)) {
        recomp_eprintf("[shinx121_debug] get_trigger_param overflow! %d/%d", offset, sizeof(buffer));
        offset = sizeof(buffer);
    }
    buffer[offset] = '\0';

    return buffer;
}

void trigger_debug_tab(Object *obj) {
    if (dbgui_begin_tab_bar("##edit")) {
        if (dbgui_begin_tab_item("Object Setup", NULL)) {
            if (obj->setup != NULL) {
                Trigger_Setup *objsetup = (Trigger_Setup*)obj->setup;
                dbgui_textf("objId: %d", objsetup->base.objId);
                dbgui_textf("quarterSize: 0x%X", objsetup->base.quarterSize);
                dbgui_textf("actExclusions1: 0x%X", objsetup->base.actExclusions1);
                dbgui_textf("loadFlags: 0x%X", objsetup->base.loadFlags);
                dbgui_textf("actExclusions2: 0x%X", objsetup->base.actExclusions2 & 0xF0);
                dbgui_textf("fadeFlags: 0x%X", objsetup->base.fadeFlags & 0x0F);
                if (objsetup->base.loadFlags & 0x10) {
                    dbgui_textf("mapObjGroup: %d", objsetup->base.mapObjGroup);
                } else {
                    dbgui_textf("loadDistance: %d", objsetup->base.loadDistance);
                }
                dbgui_textf("fadeDistance: %d", objsetup->base.fadeDistance);
                dbgui_textf("x: %f", objsetup->base.x);
                dbgui_textf("y: %f", objsetup->base.y);
                dbgui_textf("z: %f", objsetup->base.z);
                dbgui_textf("uID: 0x%X", objsetup->base.uID);

                dbgui_text("commands:");
                for (s32 i = 0; i < 8; i++) {
                    TriggerCommand *cmd = &objsetup->commands[i];
                    const char *label = cmd->id == 0
                        ? recomp_sprintf_helper("[%d] <none>###%d", i, i)
                        : recomp_sprintf_helper("[%d] %s (%s) %s###%d", i, 
                            get_trigger_name(cmd->id),
                            get_trigger_cond_name(cmd->condition),
                            get_trigger_param(cmd),
                            i);
                    if (dbgui_tree_node(label)) {
                        dbgui_input_byte_ext("Condition", &cmd->condition, &hexInput);
                        dbgui_input_byte_ext("ID", &cmd->condition, &hexInput);
                        dbgui_input_byte_ext("Param 1", &cmd->param1, &hexInput);
                        dbgui_input_byte_ext("Param 2", &cmd->param2, &hexInput);
                        dbgui_tree_pop();
                    }
                }

                dbgui_textf("localID: %d", objsetup->localID);
                dbgui_input_byte("sizeX", &objsetup->sizeX);
                dbgui_input_byte("sizeY", &objsetup->sizeY);
                dbgui_input_byte("sizeZ", &objsetup->sizeZ);
                dbgui_textf("rotationY: %d", objsetup->rotationY);
                dbgui_textf("rotationX: %d", objsetup->rotationX);
                dbgui_textf("activatorObjType: %d", objsetup->activatorObjType);
                dbgui_textf("bitFlagID: %x", objsetup->bitFlagID);
                dbgui_textf("timerDuration: %d", objsetup->timerDuration);
                dbgui_textf("conditionBitFlagIDs[0]: %x", objsetup->conditionBitFlagIDs[0]);
                dbgui_textf("conditionBitFlagIDs[1]: %x", objsetup->conditionBitFlagIDs[1]);
                dbgui_textf("conditionBitFlagIDs[2]: %x", objsetup->conditionBitFlagIDs[2]);
                dbgui_textf("conditionBitFlagIDs[3]: %x", objsetup->conditionBitFlagIDs[3]);
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
    Vec3f position = obj->prevGlobalPosition;

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
