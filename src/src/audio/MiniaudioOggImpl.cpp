#pragma once
// 用来开启miniaudio解码ogg的文件
#define STB_VORBIS_HEADER_ONLY
#include "src/thirdParty/stbvorbis/stb_vorbis.c"
#define MINIAUDIO_IMPLEMENTATION
#include "src/thirdParty/miniaudio/miniaudio.h"
#undef STB_VORBIS_HEADER_ONLY
#include "src/thirdParty/stbvorbis/stb_vorbis.c"

// 防止出现各种奇怪的宏替换导致的问题
#ifdef C
#undef C
#endif

#ifdef R
#undef R
#endif

#ifdef L
#undef L
#endif
