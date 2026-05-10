//
// Created by kkplay on 4/22/26.
//

#pragma once

#include "data/BaseRegistry.h"

class LevelRegistry : public BaseRegistry<LevelRegistry>
{
public:
    LevelRegistry() : BaseRegistry("LevelRegistry") {}
};
