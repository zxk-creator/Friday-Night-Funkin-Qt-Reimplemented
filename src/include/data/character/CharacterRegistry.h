//
// Created by kkplay on 5/2/26.
//

#pragma once
#include "data/BaseRegistry.h"

class CharacterRegistry : public BaseRegistry<CharacterRegistry>
{
public:
    CharacterRegistry() : BaseRegistry("CharacterRegistry"){}
};
