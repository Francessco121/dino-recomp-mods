#pragma once

#include "PR/ultratypes.h"

_Bool blocks_ext_try_read_tab(void **outTab);
_Bool blocks_ext_try_read_bin(void *dst, u32 offset, u32 size);
