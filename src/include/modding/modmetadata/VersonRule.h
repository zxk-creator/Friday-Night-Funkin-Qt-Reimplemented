//
// Created by kkplay on 4/22/26.
//

#pragma once

#include <string>
#include "semver/semver.h"

using std::string;

// 专门比较版本的
class VersionRules
{
public:
    string version;

    VersionRules() = default;

    explicit VersionRules(const string& version)
    {
        this->version = version;
    }

    VersionRules& operator=(const string& newVersion)
    {
        version = newVersion;
        return *this;
    }
};