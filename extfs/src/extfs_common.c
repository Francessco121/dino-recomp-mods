#include "extfs_common.h"

#include "recompconfig.h"
#include "recomputils.h"

typedef enum {
    DEBUG_LOGGING_ON,
    DEBUG_LOGGING_OFF,
} DebugLogging;

ExtFsLoadStage extfsLoadStage = EXTFS_STAGE_UNINITIALIZED;

const char *DINO_FS_FILENAMES[NUM_FILES] = {
    "AUDIO.tab",
    "AUDIO.bin",
    "SFX.tab",
    "SFX.bin",
    "AMBIENT.tab",
    "AMBIENT.bin",
    "MUSIC.tab",
    "MUSIC.bin",
    "MPEG.tab",
    "MPEG.bin",
    "MUSICACTIONS.bin",
    "CAMACTIONS.bin",
    "LACTIONS.bin",
    "ANIMCURVES.bin",
    "ANIMCURVES.tab",
    "OBJSEQ2CURVE.tab",
    "FONTS.bin",
    "CACHEFON.bin",
    "CACHEFON2.bin",
    "GAMETEXT.bin",
    "GAMETEXT.tab",
    "GLOBALMAP.bin",
    "TABLES.bin",
    "TABLES.tab",
    "SCREENS.bin",
    "SCREENS.tab",
    "VOXMAP.bin",
    "VOXMAP.tab",
    "TEXPRE.tab",
    "TEXPRE.bin",
    "WARPTAB.bin",
    "MAPS.bin",
    "MAPS.tab",
    "MAPINFO.bin",
    "MAPSETUP.ind",
    "MAPSETUP.tab",
    "TEX1.bin",
    "TEX1.tab",
    "TEXTABLE.bin",
    "TEX0.bin",
    "TEX0.tab",
    "BLOCKS.bin",
    "BLOCKS.tab",
    "TRKBLK.bin",
    "HITS.bin",
    "HITS.tab",
    "MODELS.tab",
    "MODELS.bin",
    "MODELIND.bin",
    "MODANIM.tab",
    "MODANIM.bin",
    "ANIM.tab",
    "ANIM.bin",
    "AMAP.tab",
    "AMAP.bin",
    "BITTABLE.bin",
    "WEAPONDATA.bin",
    "VOXOBJ.tab",
    "VOXOBJ.bin",
    "MODLINES.bin",
    "MODLINES.tab",
    "SAVEGAME.bin",
    "SAVEGAME.tab",
    "OBJSEQ.bin",
    "OBJSEQ.tab",
    "OBJECTS.tab",
    "OBJECTS.bin",
    "OBJINDEX.bin",
    "OBJEVENT.bin",
    "OBJHITS.bin",
    "DLLS.bin",
    "DLLS.tab",
    "DLLSIMPORTTAB.bin",
    "ENVFXACT.bin"
};

void extfs_assert(_Bool condition, const char *fmt, ...) {
    va_list args;
	va_start(args, fmt);

    if (!condition) {
        recomp_exit_with_error(recomp_vsprintf_helper(fmt, args));
    }

    va_end(args);
}

void extfs_log(const char *fmt, ...) {
    va_list args;
	va_start(args, fmt);

    s32 enableDebugLogging = recomp_get_config_u32("logging") == DEBUG_LOGGING_ON;
    if (enableDebugLogging) {
        recomp_vprintf(fmt, args);
    }

    va_end(args);
}

void extfs_log_error(const char *fmt, ...) {
    va_list args;
	va_start(args, fmt);

    recomp_veprintf(fmt, args);

    va_end(args);
}

void extfs_error_box(const char *fmt, ...) {
    va_list args;
	va_start(args, fmt);

    recomp_error_message_box(recomp_vsprintf_helper(fmt, args));

    va_end(args);
}
