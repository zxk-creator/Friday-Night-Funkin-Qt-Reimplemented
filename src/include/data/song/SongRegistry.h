//
// Created by kkplay on 4/25/26.
//

#pragma once
#include "data/BaseRegistry.h"
#include "play/song/Song.h"
#include "utils/message/MessageHandler.h"

// 专门管Song对象存放的
class SongRegistry : public BaseRegistry<SongRegistry>
{
public:
    SongRegistry() : BaseRegistry("SongRegistry") {}
};
