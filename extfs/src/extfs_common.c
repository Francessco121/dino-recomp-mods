#include "extfs_common.h"

#include "recompconfig.h"
#include "recomputils.h"

typedef enum {
    DEBUG_LOGGING_ON,
    DEBUG_LOGGING_OFF,
} DebugLogging;

ExtFsLoadStage extfsLoadStage = EXTFS_STAGE_UNINITIALIZED;

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
