//
// Created by kkplay on 4/22/26.
//

#ifndef FNF_MODENGINETYPE_H
#define FNF_MODENGINETYPE_H

#endif //FNF_MODENGINETYPE_H

#pragma once

/**
 * @brief 虽然说只想兼容PE和VS，但是万一做了呢？所以暂时保留接口！
*/
enum class ModEngineType
{
    // Psych Engine
    PE,
    // V-Slice
    VS,
    // Kade Engine
    KE,
    // Codename Engine
    CNE,
    // 反正肯定不是V-Slice
    Unknown,
};