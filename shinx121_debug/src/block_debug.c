#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "3d.h"
#include "debug_common.h"
#include "debug_menus.h"

#include "sys/fs.h"
#include "sys/memory.h"
#include "sys/map.h"
#include "common.h"

extern Block **gLoadedBlocks;
extern u8 gLoadedBlockCount;
extern s16 *gLoadedBlockIds;

typedef struct {
    f32 x;
    f32 z;
    _Bool set;
} BlockPosition;

static s32 blockDebugWindowOpen = FALSE;
static BlockPosition blockPositions[40];
static s32 showInWorld = FALSE;
static s32 show3DHits = FALSE;
static s32 show3DShapes = FALSE;
static s32 hoveredBlock = -1;
static s32 hoveredBlockLabel = FALSE;
static s32 hoveredHit = -1;
static s32 hoveredShape = -1;
static s32 filterEnabled = FALSE;
static s32 blockIDFilter = -1;
static u32 shapeFlagsFilter = 0;

static const DbgUiInputIntOptions defaultInput = {
    .step = 1,
    .stepFast = 100,
    .flags = 0
};
static const DbgUiInputIntOptions noStepInput = {
    .step = 0,
    .stepFast = 0,
    .flags = 0
};
static const DbgUiInputIntOptions hexInput = {
    .step = 0,
    .stepFast = 0,
    .flags = DBGUI_INPUT_TEXT_FLAGS_CharsHexadecimal
};

// HACK: hijack block world positions from block_add_to_render_list since idk how to calculate these
RECOMP_HOOK("block_add_to_render_list") void block_add_to_render_list(Block *block, f32 x, f32 y) {
    s32 idx = -1;
    for (s32 i = 0; i < gLoadedBlockCount; i++) {
        if (gLoadedBlocks[i] == block) {
            idx = i;
            break;
        }
    }

    if (idx == -1) {
        return;
    }

    blockPositions[idx].x = x + gWorldX;
    blockPositions[idx].z = y + gWorldZ;
    blockPositions[idx].set = TRUE;
}

extern u8 *gBlockRefCounts;

static void block_debug(Block *block, s16 id, s32 idx) {
    if (dbgui_tree_node(recomp_sprintf_helper("Block %d###%d", id, id))) {
        if (dbgui_is_item_hovered()) {
            hoveredBlock = idx;
            hoveredBlockLabel = TRUE;
        }

        if (dbgui_tree_node("Details")) {
            dbgui_textf("unk1C: %d", block->unk1C);
            dbgui_textf("texAnims: %p", block->texAnims);
            dbgui_textf("vtxFlags: %x", block->vtxFlags);
            dbgui_textf("vtxCount: %d", block->vtxCount);
            dbgui_textf("unk34: %d", block->unk34);
            dbgui_textf("shapeCount: %d", block->shapeCount);
            dbgui_textf("hits_line_count: %d", block->hits_line_count);
            dbgui_textf("unk3A: %d", block->unk3A);
            dbgui_textf("unk3B: %d", block->unk3B);
            dbgui_textf("unk3C: %d", block->unk3C);
            dbgui_textf("unk3E: %d", block->unk3E);
            dbgui_textf("minY: %d", block->minY);
            dbgui_textf("maxY: %d", block->maxY);
            dbgui_textf("modelSize: %d", block->modelSize);
            dbgui_textf("textureLoadCount: %d", block->textureLoadCount);
            dbgui_textf("numTexAnims: %u", block->numTexAnims);
            dbgui_textf("numSphereMappedShapes: %d", block->numSphereMappedShapes);
            dbgui_textf("materialCount: %u", block->materialCount);
            dbgui_textf("unk4B: %d", block->unk4B);
            dbgui_textf("unk4E: %d", block->unk4E);

            dbgui_tree_pop();
        }

        if (dbgui_tree_node("Hits")) {
            for (s32 i = 0; i < block->hits_line_count; i++) {
                HitsLineCustom *hitLine = (HitsLineCustom*)&block->ptr_hits_lines[i];
    
                if (dbgui_tree_node(recomp_sprintf_helper("%d", i))) {
                    if (dbgui_is_item_hovered()) {
                        hoveredBlock = idx;
                        hoveredHit = i;
                    }

                    dbgui_set_next_item_width(50);
                    dbgui_input_short_ext("Ax", &hitLine->Ax, &noStepInput);
                    dbgui_same_line();
                    dbgui_set_next_item_width(50);
                    dbgui_input_short_ext("Ay", &hitLine->Ay, &noStepInput);
                    dbgui_same_line();
                    dbgui_set_next_item_width(50);
                    dbgui_input_short_ext("Az", &hitLine->Az, &noStepInput);

                    dbgui_set_next_item_width(50);
                    dbgui_input_short_ext("Bx", &hitLine->Bx, &noStepInput);
                    dbgui_same_line();
                    dbgui_set_next_item_width(50);
                    dbgui_input_short_ext("By", &hitLine->By, &noStepInput);
                    dbgui_same_line();
                    dbgui_set_next_item_width(50);
                    dbgui_input_short_ext("Bz", &hitLine->Bz, &noStepInput);

                    if (dbgui_button("Swap A & B")) {
                        s16 ax, ay, az;
                        ax = hitLine->Ax;
                        ay = hitLine->Ay;
                        az = hitLine->Az;

                        hitLine->Ax = hitLine->Bx;
                        hitLine->Ay = hitLine->By;
                        hitLine->Az = hitLine->Bz;
                        hitLine->Bx = ax;
                        hitLine->By = ay;
                        hitLine->Bz = az;
                    }

                    if (hitLine->settingsA & 0x80) {
                        dbgui_set_next_item_width(130);
                        dbgui_input_short_ext("height", &hitLine->height, &defaultInput);
                    } else {
                        dbgui_set_next_item_width(130);
                        dbgui_input_sbyte_ext("heightA", &hitLine->heightA, &defaultInput);
                        dbgui_set_next_item_width(130);
                        dbgui_input_sbyte_ext("heightB", &hitLine->heightB, &defaultInput);
                    }
                    dbgui_set_next_item_width(130);
                    dbgui_input_byte_ext("settingsA", (u8*)&hitLine->settingsA, &hexInput);
                    dbgui_set_next_item_width(130);
                    dbgui_input_byte_ext("settingsB", (u8*)&hitLine->settingsB, &hexInput);
                    dbgui_textf("animatorID: %d", hitLine->animatorID);

                    dbgui_new_line();
                    if (dbgui_button("Apply Edits")) {
                        func_80058F3C();
                    }
    
                    dbgui_tree_pop();
                } else {
                    if (dbgui_is_item_hovered()) {
                        hoveredBlock = idx;
                        hoveredHit = i;
                    }
                }
            }

            dbgui_tree_pop();
        }

        if (dbgui_tree_node("Shapes")) {
            for (s32 i = 0; i < block->shapeCount; i++) {
                BlockShape *shape = &block->shapes[i];

                if (shapeFlagsFilter != 0 && !(shape->flags & shapeFlagsFilter)) {
                    continue;
                }
    
                if (dbgui_tree_node(recomp_sprintf_helper("%d", i))) {
                    if (dbgui_is_item_hovered()) {
                        hoveredBlock = idx;
                        hoveredShape = i;
                    }

                    dbgui_textf("flags: 0x%X", shape->flags);
                    dbgui_textf("vtxBase: %d", shape->vtxBase);
                    dbgui_textf("triBase: %d", shape->triBase);
                    dbgui_textf("Ymin: %d", shape->Ymin);
                    dbgui_textf("Ymax: %d", shape->Ymax);
                    dbgui_textf("Xmin: %d", shape->Xmin);
                    dbgui_textf("Xmax: %d", shape->Xmax);
                    dbgui_textf("Zmin: %d", shape->Zmin);
                    dbgui_textf("Zmax: %d", shape->Zmax);
                    dbgui_textf("unk10: %d", shape->unk10);
                    dbgui_textf("unk11: %d", shape->unk11);
                    dbgui_textf("materialIndex: %d", shape->materialIndex);
                    dbgui_textf("envColourMode: %d", shape->envColourMode);
                    dbgui_textf("animatorID: %d", shape->animatorID);
                    dbgui_textf("blendMaterialIndex: %d", shape->blendMaterialIndex);
                    dbgui_textf("texScrollerID: %d", shape->texScrollerID);
                    dbgui_textf("boundsRemainder: %d", shape->boundsRemainder);
    
                    dbgui_tree_pop();
                } else {
                    if (dbgui_is_item_hovered()) {
                        hoveredBlock = idx;
                        hoveredShape = i;
                    }
                }
            }

            dbgui_tree_pop();
        }

        dbgui_tree_pop();
    } else {
        if (dbgui_is_item_hovered()) {
            hoveredBlock = idx;
            hoveredBlockLabel = TRUE;
        }
    }
}

static void block_debug_3d(Block *block, s16 id, s32 idx) {
    BlockPosition *pos = &blockPositions[idx];
    if (pos->set) {
        pos->set = FALSE;

        s32 _hoveredBlock = hoveredBlock == idx;

        if (show3DHits) {
            for (s32 i = 0; i < block->hits_line_count; i++) {
                HitsLineCustom *hitLine = (HitsLineCustom*)&block->ptr_hits_lines[i];

                s32 _hoveredHit = _hoveredBlock && (hoveredHit == i || (hoveredBlockLabel && hoveredHit == -1));

                f32 centerX = (f32)(hitLine->Ax + hitLine->Bx) / 2;
                f32 centerY = (f32)(hitLine->Ay + hitLine->By) / 2;
                f32 centerZ = (f32)(hitLine->Az + hitLine->Bz) / 2;

                s32 heightA, heightB;
                if (hitLine->settingsA & 0x80) {
                    heightA = hitLine->height;
                    heightB = hitLine->height;
                } else {
                    heightA = hitLine->heightA;
                    heightB = hitLine->heightB;
                }

                draw_3d_text(pos->x + centerX,  centerY, pos->z + centerZ, 
                    recomp_sprintf_helper("Hit %d/%d", id, i), 
                    _hoveredHit ? 0xFFFFFFFF : 0xFF00FFFF);
                
                draw_3d_line(
                    pos->x + hitLine->Ax, hitLine->Ay, pos->z + hitLine->Az, 
                    pos->x + hitLine->Bx, hitLine->By, pos->z + hitLine->Bz, 
                    _hoveredHit ? 0xFFFFFFFF : 0xFFFF00FF);

                draw_3d_line(
                    pos->x + hitLine->Ax, hitLine->Ay, pos->z + hitLine->Az, 
                    pos->x + hitLine->Ax, hitLine->Ay + heightA, pos->z + hitLine->Az, 
                    _hoveredHit ? 0xFFFFFFFF : 0x888800CC);

                draw_3d_line(
                    pos->x + hitLine->Bx, hitLine->By, pos->z + hitLine->Bz, 
                    pos->x + hitLine->Bx, hitLine->By + heightB, pos->z + hitLine->Bz, 
                    _hoveredHit ? 0xFFFFFFFF : 0x888800CC);

                draw_3d_line(
                    pos->x + hitLine->Ax, hitLine->Ay + heightA, pos->z + hitLine->Az, 
                    pos->x + hitLine->Bx, hitLine->By + heightB, pos->z + hitLine->Bz, 
                    _hoveredHit ? 0xFFFFFFFF : 0x888800CC);
            }
        }

        draw_3d_text(pos->x, block->minY, pos->z, recomp_sprintf_helper("Block %d", id), 
            _hoveredBlock ? 0xFFFFFFFF : 0xFF00FFFF);

        if (show3DShapes) {
            for (s32 i = 0; i < block->shapeCount; i++) {
                BlockShape *shape = &block->shapes[i];

                if (!(shape->flags & 0x10000000)) {
                    continue;
                }
                if (shapeFlagsFilter != 0 && !(shape->flags & shapeFlagsFilter)) {
                    continue;
                }

                s32 _hoveredShape = _hoveredBlock && (hoveredShape == i || (hoveredBlockLabel && hoveredShape == -1));

                Vtx_t *verts = &block->vertices2[block->vtxFlags & 1][shape->vtxBase];
                Vtx_t *tri[3];
                f32 center[3];
                s32 totalVerts = 0;

                for (s32 triIdx = shape->triBase; triIdx < shape[1].triBase; triIdx++) {
                    tri[0] = &verts[(block->encodedTris[triIdx].d0 >> 13) & 0x1F];
                    tri[1] = &verts[(block->encodedTris[triIdx].d0 >> 7) & 0x1F];
                    tri[2] = &verts[(block->encodedTris[triIdx].d0 >> 1) & 0x1F];
                    f32 vx[3];
                    f32 vy[3];
                    f32 vz[3];
                    for (s32 j = 0; j < 3; j++) {
                        s32 x = tri[j]->ob[0];
                        s32 y = tri[j]->ob[1];
                        s32 z = tri[j]->ob[2];
                        if (shape->flags & 0x20000000) {
                            x *= 1.0f;
                            y *= 0.05f;
                            z *= 1.0f;
                            y += block->minY;
                        }

                        vx[j] = x + pos->x;
                        vy[j] = y;
                        vz[j] = z + pos->z;

                        center[0] += vx[j];
                        center[1] += vy[j];
                        center[2] += vz[j];
                        totalVerts++;
                    }

                    draw_3d_line(vx[0], vy[0], vz[0], vx[1], vy[1], vz[1], _hoveredShape ? 0xFFFFFFFF : 0xFFFF00FF);
                    draw_3d_line(vx[1], vy[1], vz[1], vx[2], vy[2], vz[2], _hoveredShape ? 0xFFFFFFFF : 0xFFFF00FF);
                    draw_3d_line(vx[2], vy[2], vz[2], vx[0], vy[0], vz[0], _hoveredShape ? 0xFFFFFFFF : 0xFFFF00FF);
                }

                // TODO: average sucks, do (min + max) / 2
                center[0] /= totalVerts;
                center[1] /= totalVerts;
                center[2] /= totalVerts;

                draw_3d_text(center[0], center[1], center[2], recomp_sprintf_helper("Shape %d/%d", id, i), 
                    _hoveredShape ? 0xFFFFFFFF : 0xFF00FFFF);
            }
        }
    }
}

static void refcount_list(void) {
    for (s32 i = 0; i < gLoadedBlockCount; i++) {
        if (gLoadedBlockIds[i] == -1) {
            continue;
        }
        dbgui_textf("Block %d: %d\n", gLoadedBlockIds[i], gBlockRefCounts[i]);
    }
}

static void loaded_blocks_list(void) {
    dbgui_checkbox("Show in world", &showInWorld);
    if (showInWorld) {
        dbgui_checkbox("Show 3D hits", &show3DHits);
        dbgui_checkbox("Show 3D shapes", &show3DShapes);
    }
    dbgui_checkbox("Enable filters", &filterEnabled);
    if (filterEnabled) {
        dbgui_push_item_width(120);
        dbgui_input_int("Block ID Filter", &blockIDFilter);
        dbgui_pop_item_width();
        dbgui_push_item_width(180);
        dbgui_input_uint_ext("Shape Flag Filter", &shapeFlagsFilter, &hexInput);
        dbgui_pop_item_width();
    } else {
        blockIDFilter = -1;
        shapeFlagsFilter = 0;
    }

    for (s32 i = 0; i < gLoadedBlockCount; i++) {
        Block *block = gLoadedBlocks[i];
        if (block != NULL && (blockIDFilter == -1 || blockIDFilter == gLoadedBlockIds[i])) {
            block_debug(block, gLoadedBlockIds[i], i);
        }
    }
}

void blocks_debug_menu_callback(void) {
    dbgui_menu_item("Blocks", &blockDebugWindowOpen);
}

RECOMP_CALLBACK(".", my_dbgui_event) void blocks_debug_dbgui_callback(void) {
    hoveredBlock = -1;
    hoveredHit = -1;
    hoveredShape = -1;
    hoveredBlockLabel = FALSE;

    if (blockDebugWindowOpen) {
        if (dbgui_begin("Blocks Debug", &blockDebugWindowOpen)) {
            if (dbgui_begin_tab_bar("tabs")) {
                if (dbgui_begin_tab_item("Loaded Blocks", NULL)) {
                    loaded_blocks_list();
                    dbgui_end_tab_item();
                }
                if (dbgui_begin_tab_item("Ref Counts", NULL)) {
                    refcount_list();
                    dbgui_end_tab_item();
                }
                dbgui_end_tab_bar();
            }
        }
        dbgui_end();
    }

    if (showInWorld) {
        for (s32 i = 0; i < gLoadedBlockCount; i++) {
            Block *block = gLoadedBlocks[i];
            if (block != NULL && (blockIDFilter == -1 || blockIDFilter == gLoadedBlockIds[i])) {
                block_debug_3d(block, gLoadedBlockIds[i], i);
            }
        }
    }
}