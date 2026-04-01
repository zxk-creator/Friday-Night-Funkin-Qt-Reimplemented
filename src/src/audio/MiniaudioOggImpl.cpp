#pragma once
// 用来开启miniaudio解码ogg的文件
#define STB_VORBIS_HEADER_ONLY
#include "src/thirdParty/stbvorbis/stb_vorbis.c"
#define MINIAUDIO_IMPLEMENTATION
#include "src/thirdParty/miniaudio/miniaudio.h"
#undef STB_VORBIS_HEADER_ONLY
#include "src/thirdParty/stbvorbis/stb_vorbis.c"

#undef STB_VORBIS_HEADER_ONLY
