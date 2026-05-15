//
// Created by kkplay on 4/26/26.
//

#pragma once
#include <QString>

namespace SongDataRelative
{
    // Song
    static inline const QString SONG_METADATA_VERSION = "2.2.4";
    static inline const QString SONG_METADATA_VERSION_RULE = "2.2.x";
    static inline const QString SONG_CHART_DATA_VERSION = "2.0.0";
    static inline const QString SONG_CHART_DATA_VERSION_RULE = "2.0.x";
    static inline const QString SONG_MUSIC_DATA_VERSION = "2.0.0";
    static inline const QString SONG_MUSIC_DATA_VERSION_RULE = "2.0.x";
}

namespace NoteStyleDataRelative
{
    // notestyle
    static inline const QString NOTE_STYLE_DATA_VERSION = "1.0.0";
    static inline const QString NOTE_STYLE_DATA_VERSION_RULE = ">=1.0.0 <1.2.0";
}

namespace CharacterDataRelative
{
    // CharacterData
    static inline const QString CHARACTER_DATA_VERSION = "1.0.1";
    static inline const QString CHARACTER_DATA_VERSION_RULE = "1.0.x";
}

namespace LevelDataRelative
{
    // Level
    static inline const QString LEVEL_DATA_VERSION = "1.0.1";
    static inline const QString LEVEL_DATA_VERSION_RULE = ">=1.0.0 <1.1.0";
}

namespace StageDataRelative
{
    // StageData
    static inline const QString STAGE_DATA_VERSION = "1.0.2";
    static inline const QString STAGE_DATA_VERSION_RULE = ">=1.0.0 <1.1.0";
}

namespace Common
{
    static inline const QString CURRENT_ENGINE_VERSION = "0.0.0.5";
    static inline const QString DEFAULT_GENERATED_BY = "Quark Engine - " + CURRENT_ENGINE_VERSION;
}