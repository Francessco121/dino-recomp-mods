#include "modding.h"
#include "dbgui.h"
#include "recomputils.h"

#include "real3d.h"

#include "PR/gbi.h"
#include "sys/map.h"
#include "sys/math.h"
#include "sys/memory.h"
#include "sys/objects.h"
#include "macros.h"

#define REAL3D_BASE_SCALE (100.0f)
#define REAL3D_BASE_SCALE_INV (1.0f/REAL3D_BASE_SCALE)

extern Gfx* gMainDL;
extern Mtx* gWorldRSPMatrices;
extern Vtx* D_800B51D4;

RECOMP_DECLARE_EVENT(real3d_event())

static Gfx* cachedSphere = NULL;

static void render_sphere(Gfx** gdl, Vtx** vtxs) {
    if (cachedSphere == NULL) {
        Gfx* gfxStart = (*gdl)++;
        cachedSphere = *gdl;

        const s32 width = 16;
        const s32 height = 16;

        gSPVertex((*gdl)++, *vtxs, width + 1, 0);
        
        {
            Vtx* vtx = (*vtxs)++;
            vtx->n.ob[0] = 0.0f * REAL3D_BASE_SCALE;
            vtx->n.ob[1] = 0.5f * REAL3D_BASE_SCALE;
            vtx->n.ob[2] = 0.0f * REAL3D_BASE_SCALE;
            vtx->n.n[0] = 0.0f;
            vtx->n.n[1] = 127.0f;
            vtx->n.n[2] = 0.0f;
            vtx->n.a = 255;
        }

        for (s32 x = 0; x < width; x++) {
            s32 v0 = 0; // ul
            s32 v1 = 0; // ur
            s32 v2 = x + 1; // ll
            s32 v3 = ((x + 1) % width) + 1; // lr

            gSP2Triangles((*gdl)++, 
                v0, v1, v2, 0,
                v1, v3, v2, 0);
        }

        for (s32 y = 1; y < height; y++) {
            f32 ySin = mathSinfInterp((M_180_DEGREES / height) * y);
            
            Vec3f pos;
            pos.y = mathCosfInterp((M_180_DEGREES / height) * y) * 0.5f;

            if (y == (height - 1)) {
                gSPVertex((*gdl)++, *vtxs, width + 1, 0);
            } else {
                gSPVertex((*gdl)++, *vtxs, width * 2, 0);
            }

            for (s32 x = 0; x < width; x++) {
                pos.x = mathCosfInterp((M_360_DEGREES / width) * x) * ySin * 0.5f;
                pos.z = mathSinfInterp((M_360_DEGREES / width) * x) * ySin * 0.5f;

                Vtx* vtx = (*vtxs)++;
                vtx->n.ob[0] = pos.x * REAL3D_BASE_SCALE;
                vtx->n.ob[1] = pos.y * REAL3D_BASE_SCALE;
                vtx->n.ob[2] = pos.z * REAL3D_BASE_SCALE;
                vtx->n.n[0] = MIN(pos.x * 2.0f * 128.0f, 127.0f);
                vtx->n.n[1] = MIN(pos.y * 2.0f * 128.0f, 127.0f);
                vtx->n.n[2] = MIN(pos.z * 2.0f * 128.0f, 127.0f);
                vtx->n.a = 255;

                if (y == (height - 1)) {
                    s32 v0 = x; // ul
                    s32 v1 = (x + 1) % width; // ur
                    s32 v2 = width; // ll
                    s32 v3 = width; // lr

                    gSP2Triangles((*gdl)++, 
                        v0, v1, v2, 0,
                        v1, v3, v2, 0);
                } else {
                    s32 v0 = x; // ul
                    s32 v1 = (x + 1) % width; // ur
                    s32 v2 = v0 + width; // ll
                    s32 v3 = v1 + width; // lr

                    gSP2Triangles((*gdl)++, 
                        v0, v1, v2, 0,
                        v1, v3, v2, 0);
                }
            }
        }

        {
            Vtx* vtx = (*vtxs)++;
            vtx->n.ob[0] = 0.0f * REAL3D_BASE_SCALE;
            vtx->n.ob[1] = -0.5f * REAL3D_BASE_SCALE;
            vtx->n.ob[2] = 0.0f * REAL3D_BASE_SCALE;
            vtx->n.n[0] = 0.0f;
            vtx->n.n[1] = -128.0f;
            vtx->n.n[2] = 0.0f;
            vtx->n.a = 255;
        }

        gSPEndDisplayList((*gdl)++);
        gSPBranchList(gfxStart, *gdl);
    }

    gSPDisplayList((*gdl)++, cachedSphere);
}

static Gfx* cachedSquare = NULL;

static void render_square(Gfx** gdl, Vtx** vtxs) {
    if (cachedSquare == NULL) {
        static const Vec3f points[] = {
            VEC3F(-0.5f, 0.5f, 0),
            VEC3F(0.5f, 0.5f, 0),
            VEC3F(-0.5f, -0.5f, 0),
            VEC3F(0.5f, -0.5f, 0)
        };

        Gfx* gfxStart = (*gdl)++;
        cachedSquare = *gdl;

        gSPVertex((*gdl)++, *vtxs, ARRAYCOUNT(points), 0);

        for (u32 i = 0; i < ARRAYCOUNT(points); i++) {
            const Vec3f* pos = &points[i];

            Vtx* vtx = (*vtxs)++;
            vtx->n.ob[0] = pos->x * REAL3D_BASE_SCALE;
            vtx->n.ob[1] = pos->y * REAL3D_BASE_SCALE;
            vtx->n.ob[2] = pos->z * REAL3D_BASE_SCALE;
            vtx->n.n[0] = MIN(pos->x * 2.0f * 128.0f, 127.0f);
            vtx->n.n[1] = MIN(pos->y * 2.0f * 128.0f, 127.0f);
            vtx->n.n[2] = MIN(pos->z * 2.0f * 128.0f, 127.0f);
            vtx->n.a = 255;
        }

        gSP2Triangles((*gdl)++, 
            0, 1, 2, 0,
            1, 3, 2, 0);

        gSPEndDisplayList((*gdl)++);
        gSPBranchList(gfxStart, *gdl);
    }

    gSPDisplayList((*gdl)++, cachedSquare);
}

static Gfx* cachedCylinder = NULL;

static void render_cylinder(Gfx** gdl, Vtx** vtxs) {
    if (cachedCylinder == NULL) {
        Gfx* gfxStart = (*gdl)++;
        cachedCylinder = *gdl;

        const s32 segments = 16;

        // Top circle
        gSPVertex((*gdl)++, *vtxs, segments + 1, 0);

        {
            Vtx* vtx = (*vtxs)++;
            vtx->n.ob[0] = 0.0f * REAL3D_BASE_SCALE;
            vtx->n.ob[1] = 0.5f * REAL3D_BASE_SCALE;
            vtx->n.ob[2] = 0.0f * REAL3D_BASE_SCALE;
            vtx->n.n[0] = 0.0f;
            vtx->n.n[1] = 127.0f;
            vtx->n.n[2] = 0.0f;
            vtx->n.a = 255;
        }

        for (s32 i = 0; i < segments; i++) {
            Vec3f pos;
            pos.y = 0.5f;
            pos.x = mathCosfInterp((M_360_DEGREES / segments) * i) * 0.5f;
            pos.z = mathSinfInterp((M_360_DEGREES / segments) * i) * 0.5f;

            Vtx* vtx = (*vtxs)++;
            vtx->n.ob[0] = pos.x * REAL3D_BASE_SCALE;
            vtx->n.ob[1] = pos.y * REAL3D_BASE_SCALE;
            vtx->n.ob[2] = pos.z * REAL3D_BASE_SCALE;
            vtx->n.n[0] = 0.0f;
            vtx->n.n[1] = 127.0f;
            vtx->n.n[2] = 0.0f;
            vtx->n.a = 255;

            s32 v0 = 0; // ul
            s32 v1 = 0; // ur
            s32 v2 = i + 1; // ll
            s32 v3 = ((i + 1) % segments) + 1; // lr

            gSP2Triangles((*gdl)++, 
                v0, v1, v2, 0,
                v1, v3, v2, 0);
        }

        // Middle
        gSPVertex((*gdl)++, *vtxs, segments * 2, 0);

        for (s32 y = 0; y < 2; y++) {
            for (s32 i = 0; i < segments; i++) {
                Vec3f pos;
                pos.y = y == 0 ? 0.5f : -0.5f;
                pos.x = mathCosfInterp((M_360_DEGREES / segments) * i) * 0.5f;
                pos.z = mathSinfInterp((M_360_DEGREES / segments) * i) * 0.5f;
    
                Vtx* vtx = (*vtxs)++;
                vtx->n.ob[0] = pos.x * REAL3D_BASE_SCALE;
                vtx->n.ob[1] = pos.y * REAL3D_BASE_SCALE;
                vtx->n.ob[2] = pos.z * REAL3D_BASE_SCALE;
                vtx->n.n[0] = 0.0f;
                vtx->n.n[1] = y == 0 ? 127.0f : -128.0f;
                vtx->n.n[2] = 0.0f;
                vtx->n.a = 255;
    
                if (y == 0) {
                    s32 v0 = i; // ul
                    s32 v1 = (i + 1) % segments; // ur
                    s32 v2 = v0 + segments; // ll
                    s32 v3 = v1 + segments; // lr

                    gSP2Triangles((*gdl)++, 
                        v0, v1, v2, 0,
                        v1, v3, v2, 0);
                }
            }
        }
        
        // Bottom circle
        gSPVertex((*gdl)++, *vtxs, segments + 1, 0);

        {
            Vtx* vtx = (*vtxs)++;
            vtx->n.ob[0] = 0.0f * REAL3D_BASE_SCALE;
            vtx->n.ob[1] = -0.5f * REAL3D_BASE_SCALE;
            vtx->n.ob[2] = 0.0f * REAL3D_BASE_SCALE;
            vtx->n.n[0] = 0.0f;
            vtx->n.n[1] = -128.0f;
            vtx->n.n[2] = 0.0f;
            vtx->n.a = 255;
        }

        for (s32 i = 0; i < segments; i++) {
            Vec3f pos;
            pos.y = -0.5f;
            pos.x = mathCosfInterp((M_360_DEGREES / segments) * i) * 0.5f;
            pos.z = mathSinfInterp((M_360_DEGREES / segments) * i) * 0.5f;

            Vtx* vtx = (*vtxs)++;
            vtx->n.ob[0] = pos.x * REAL3D_BASE_SCALE;
            vtx->n.ob[1] = pos.y * REAL3D_BASE_SCALE;
            vtx->n.ob[2] = pos.z * REAL3D_BASE_SCALE;
            vtx->n.n[0] = 0.0f;
            vtx->n.n[1] = -128.0f;
            vtx->n.n[2] = 0.0f;
            vtx->n.a = 255;

            s32 v0 = 0; // ul
            s32 v1 = 0; // ur
            s32 v2 = i + 1; // ll
            s32 v3 = ((i + 1) % segments) + 1; // lr

            gSP2Triangles((*gdl)++, 
                v0, v1, v2, 0,
                v1, v3, v2, 0);
        }

        gSPEndDisplayList((*gdl)++);
        gSPBranchList(gfxStart, *gdl);
    }

    gSPDisplayList((*gdl)++, cachedCylinder);
}

static Gfx* cachedCube = NULL;

static void render_cube_face(
        Gfx** gdl, Vtx** vtxs, 
        const Vec3f* points, 
        s32 vbase,
        s32 ul, s32 ur,
        s32 ll, s32 lr,
        f32 nx, f32 ny, f32 nz) {
    for (u32 i = 0; i < 4; i++) {
        const Vec3f* pos;

        switch (i) {
        case 0:
            pos = &points[ul];
            break;
        case 1:
            pos = &points[ur];
            break;
        case 2:
            pos = &points[ll];
            break;
        case 3:
            pos = &points[lr];
            break;
        }

        Vtx* vtx = (*vtxs)++;
        vtx->n.ob[0] = pos->x * REAL3D_BASE_SCALE;
        vtx->n.ob[1] = pos->y * REAL3D_BASE_SCALE;
        vtx->n.ob[2] = pos->z * REAL3D_BASE_SCALE;
        vtx->n.n[0] = nx;
        vtx->n.n[1] = ny;
        vtx->n.n[2] = nz;
        vtx->n.a = 255;
    }

    gSP2Triangles((*gdl)++, 
        vbase + 0, vbase + 1, vbase + 2, 0,
        vbase + 1, vbase + 3, vbase + 2, 0);
}

static void render_cube(Gfx** gdl, Vtx** vtxs) {
    if (cachedCube == NULL) {
        Gfx* gfxStart = (*gdl)++;
        cachedCube = *gdl;

        static const Vec3f points[] = {
            VEC3F(-0.5f,  0.5f, -0.5f),
            VEC3F( 0.5f,  0.5f, -0.5f),
            VEC3F(-0.5f,  0.5f,  0.5f),
            VEC3F( 0.5f,  0.5f,  0.5f),
            VEC3F(-0.5f, -0.5f, -0.5f),
            VEC3F( 0.5f, -0.5f, -0.5f),
            VEC3F(-0.5f, -0.5f,  0.5f),
            VEC3F( 0.5f, -0.5f,  0.5f),
        };

        gSPVertex((*gdl)++, *vtxs, 4 * 6, 0);

        // Top
        render_cube_face(gdl, vtxs, points, 0 * 4, 
            0, 1, 2, 3,
            0.0f, 127.0f, 0.0f);
        // Bottom
        render_cube_face(gdl, vtxs, points, 1 * 4, 
            4, 5, 6, 7,
            0.0f, -128.0f, 0.0f);
        // Front
        render_cube_face(gdl, vtxs, points, 2 * 4, 
            2, 3, 6, 7,
            0.0f, 0.0f, 127.0f);
        // Back
        render_cube_face(gdl, vtxs, points, 3 * 4, 
            0, 1, 4, 5,
            0.0f, 0.0f, -128.0f);
        // Left
        render_cube_face(gdl, vtxs, points, 4 * 4, 
            0, 2, 4, 6,
            -128.0f, 0.0f, 0.0f);
        // Right
        render_cube_face(gdl, vtxs, points, 5 * 4, 
            1, 3, 5, 7,
            127.0f, 0.0f, 0.0f);

        gSPEndDisplayList((*gdl)++);
        gSPBranchList(gfxStart, *gdl);
    }

    gSPDisplayList((*gdl)++, cachedCube);
}

static void decode_argb(u32 argb, u8* r, u8* g, u8* b, u8* a) {
    *a = (argb & 0xFF000000) >> 24;
    *r = (argb & 0x00FF0000) >> 16;
    *g = (argb & 0x0000FF00) >> 8;
    *b = (argb & 0x000000FF) >> 0;
}

static void set_matrix(Gfx** gdl, Mtx** mtxs, SRT* srt, f32 scaleX, f32 scaleY, f32 scaleZ) {
    srt->transl.x -= gWorldX;
    srt->transl.z -= gWorldZ;

    if (srt->transl.x > 32767.0f || -32767.0f > srt->transl.x || srt->transl.z > 32767.0f || -32767.0f > srt->transl.z) {
        recomp_eprintf("[real3d] set_matrix SRT coords out of bounds\n");
        gSPMatrix((*gdl)++, OS_K0_TO_PHYSICAL(mathIdentityMtxL()), G_MTX_LOAD | G_MTX_MODELVIEW);

        srt->transl.x += gWorldX;
        srt->transl.z += gWorldZ;
        return;
    }

    MtxF mtx;
    mathYprXyzMtx(&mtx, srt);
    if (scaleX != 1.0f || scaleY != 1.0f || scaleZ != 1.0f) {
        MtxF scaleMtx;
        mathScaleMtx(&scaleMtx, scaleX, scaleY, scaleZ);
        mathMtxCatF(&scaleMtx, &mtx, &mtx);
    }

    mathMtx4x3F2L(&mtx, *mtxs);
    gSPMatrix((*gdl)++, OS_K0_TO_PHYSICAL((*mtxs)++), G_MTX_LOAD | G_MTX_MODELVIEW);

    srt->transl.x += gWorldX;
    srt->transl.z += gWorldZ;
}

static void gfx_begin(Gfx** gdl) {
    static Lights2 myLight = gdSPDefLights2(
        128, 128, 128,
        255, 255, 255, -80, -80, 0,
        255, 255, 255, 80, 80, 0
    );
    static Lights2* myLightMem = NULL;
    if (myLightMem == NULL) {
        myLightMem = mmAlloc(sizeof(*myLightMem), 0, NULL);
        *myLightMem = myLight;
    }

    gSPSetLights2((*gdl)++, (*myLightMem));

    gSPGeometryMode(*gdl, 0xFFFFFF, G_ZBUFFER | G_SHADE | G_LIGHTING | G_SHADING_SMOOTH);
    dlApplyGeometryMode(gdl);
    gDPSetCombineMode(*gdl, G_CC_DINO_SHADEPRIMRGB_PRIMA, G_CC_PASS2);
    dlApplyCombine(gdl);
    gDPSetOtherMode(*gdl, 
        G_AD_PATTERN | G_CD_NOISE | G_CK_NONE | G_TC_FILT | G_TF_BILERP | G_TT_NONE | G_TL_TILE | G_TD_CLAMP | G_TP_PERSP | G_CYC_2CYCLE | G_PM_NPRIMITIVE, 
        G_AC_NONE | G_ZS_PIXEL | G_RM_NOOP | G_RM_ZB_XLU_SURF2);
    dlApplyOtherMode(gdl);
}

static void gfx_end(Gfx** gdl) {
    dlClearGeometryMode(gdl, G_LIGHTING | G_SHADING_SMOOTH);
}

void real3d_draw_sphere(f32 x, f32 y, f32 z, f32 scale, u32 argb) {
    Gfx** gdl = &gMainDL;
    Mtx** mtxs = &gWorldRSPMatrices;
    Vtx** vtxs = &D_800B51D4;

    u8 r, g, b, a;
    decode_argb(argb, &r, &g, &b, &a);

    SRT srt = {
        .yaw = 0,
        .pitch = 0,
        .roll = 0,
        .flags = 0,
        .scale = scale * REAL3D_BASE_SCALE_INV,
        .transl = {
            .x = x,
            .y = y,
            .z = z
        }
    };

    gfx_begin(gdl);

    dlSetPrimColor(gdl, r, g, b, a);
    set_matrix(gdl, mtxs, &srt, 1.0f, 1.0f, 1.0f);
    render_sphere(gdl, vtxs);
    
    gfx_end(gdl);
}

void real3d_draw_rect(SRT* srt, f32 width, f32 height, u32 argb) {
    Gfx** gdl = &gMainDL;
    Mtx** mtxs = &gWorldRSPMatrices;
    Vtx** vtxs = &D_800B51D4;

    u8 r, g, b, a;
    decode_argb(argb, &r, &g, &b, &a);

    SRT actualSRT = *srt;
    actualSRT.scale *= REAL3D_BASE_SCALE_INV;

    gfx_begin(gdl);

    dlSetPrimColor(gdl, r, g, b, a);
    set_matrix(gdl, mtxs, &actualSRT, width, height, 1.0f);
    render_square(gdl, vtxs);
    
    gfx_end(gdl);
}

void real3d_draw_cylinder(SRT* srt, f32 width, f32 height, u32 argb) {
    Gfx** gdl = &gMainDL;
    Mtx** mtxs = &gWorldRSPMatrices;
    Vtx** vtxs = &D_800B51D4;

    u8 r, g, b, a;
    decode_argb(argb, &r, &g, &b, &a);

    SRT actualSRT = *srt;
    actualSRT.scale *= REAL3D_BASE_SCALE_INV;

    gfx_begin(gdl);

    dlSetPrimColor(gdl, r, g, b, a);
    set_matrix(gdl, mtxs, &actualSRT, width, height, width);
    render_cylinder(gdl, vtxs);
    
    gfx_end(gdl);
}

void real3d_draw_box(SRT* srt, f32 width, f32 height, f32 depth, u32 argb) {
    Gfx** gdl = &gMainDL;
    Mtx** mtxs = &gWorldRSPMatrices;
    Vtx** vtxs = &D_800B51D4;

    u8 r, g, b, a;
    decode_argb(argb, &r, &g, &b, &a);

    SRT actualSRT = *srt;
    actualSRT.scale *= REAL3D_BASE_SCALE_INV;

    gfx_begin(gdl);

    dlSetPrimColor(gdl, r, g, b, a);
    set_matrix(gdl, mtxs, &actualSRT, width, height, depth);
    render_cube(gdl, vtxs);
    
    gfx_end(gdl);
}

RECOMP_HOOK_RETURN("trackDrawMain") void real3d_draw_hook(void) {
    cachedSphere = NULL;
    cachedSquare = NULL;
    cachedCylinder = NULL;
    cachedCube = NULL;

    if (!dbgui_is_open()) {
        return;
    }

    real3d_event();
}
