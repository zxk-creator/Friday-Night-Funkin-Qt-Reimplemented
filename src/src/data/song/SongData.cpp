#include "data/song/SongData.h"

#include "utils/message/MessageHandler.h"

using namespace std;

// ==================== 版本迁移逻辑 ====================

static SongMetaData migrate_v2_0_to_v2_2(const SongMetadata_v2_0_0& old) {
    SongMetaData m;
    m.version = "2.2.4";
    m.songName = old.songName;
    m.artist = old.artist;
    m.charter = old.charter;
    m.divisions = old.divisions;
    m.looped = old.looped;
    m.offsets = old.offsets;
    m.generatedBy = old.generatedBy;
    m.timeFormat = old.timeFormat;
    m.timeChanges = old.timeChanges;

    // 将顶层字段迁移到 playData
    m.playData.difficulties = old.difficulties;
    m.playData.characters = old.characters;
    m.playData.stage = old.stage;
    m.playData.noteStyle = old.noteStyle;
    m.playData.ratings = old.ratings;

    // 2.2.x 新增字段，设置默认值
    m.playData.songVariations = {};
    m.playData.previewStart = 0;
    m.playData.previewEnd = 15000;
    m.playData.album = std::nullopt;
    m.playData.stickerPack = std::nullopt;

    return m;
}

static SongMetaData migrate_v2_1_to_v2_2(const SongMetadata_v2_1_0& old) {
    SongMetaData m;
    m.version = "2.2.4";
    m.songName = old.songName;
    m.artist = old.artist;
    m.charter = old.charter;
    m.divisions = old.divisions;
    m.looped = old.looped;
    m.offsets = old.offsets;
    m.playData = old.playData;
    m.generatedBy = old.generatedBy;
    m.timeFormat = old.timeFormat;
    m.timeChanges = old.timeChanges;

    // 补全 2.2.x 新增的字段（如果缺失）
    if (!m.playData.album.has_value()) {
        m.playData.album = std::nullopt;
    }
    if (!m.playData.stickerPack.has_value()) {
        m.playData.stickerPack = std::nullopt;
    }

    return m;
}

static SongMetaData parseMetadata_v2_2_x(const json& j) {
    SongMetaData m;
    m.from_json(j);
    return m;
}

void SongMetadata_v2_1_0::from_json(const json& j)
{
    if (j.contains("version")) version = QString::fromStdString(j["version"].get<std::string>());
    if (j.contains("songName")) songName = QString::fromStdString(j["songName"].get<std::string>());
    if (j.contains("artist")) artist = QString::fromStdString(j["artist"].get<std::string>());
    if (j.contains("charter")) charter = QString::fromStdString(j["charter"].get<std::string>());
    if (j.contains("divisions")) divisions = j["divisions"].get<int>();
    if (j.contains("looped")) looped = j["looped"].get<bool>();
    if (j.contains("offsets")) offsets.from_json(j["offsets"]);
    if (j.contains("playData")) playData.from_json(j["playData"]);
    if (j.contains("generatedBy")) generatedBy = QString::fromStdString(j["generatedBy"].get<std::string>());
    if (j.contains("timeFormat")) {
        std::string s = j["timeFormat"].get<std::string>();
        if (s == "ticks") timeFormat = SongTimeFormat::TICKS;
        else if (s == "float") timeFormat = SongTimeFormat::FLOAT;
        else timeFormat = SongTimeFormat::MILLISECONDS;
    }
    if (j.contains("timeChanges") && j["timeChanges"].is_array()) {
        timeChanges.clear();
        for (const auto& item : j["timeChanges"]) {
            SongTimeChange t;
            t.from_json(item);
            timeChanges.push_back(t);
        }
    }
}

void SongChartData::from_json(const json& j)
{
    // version
    if (j.contains("version")) {
        version = QString::fromStdString(j["version"].get<std::string>());
    }

    // generatedBy
    if (j.contains("generatedBy")) {
        generatedBy = QString::fromStdString(j["generatedBy"].get<std::string>());
    }

    // scrollSpeed: Map<String, Float>
    if (j.contains("scrollSpeed") && j["scrollSpeed"].is_object()) {
        for (auto& [key, val] : j["scrollSpeed"].items()) {
            scrollSpeed[QString::fromStdString(key)] = val.get<float>();
        }
    }

    // events: Array<SongEventData>
    if (j.contains("events") && j["events"].is_array()) {
        events.clear();
        for (const auto& item : j["events"]) {
            SongEventData e;
            e.from_json(item);
            events.push_back(e);
        }
    }

    // notes: Map<String, Array<SongNoteData>>
    // 我们也不用手动操作Difficulty结构了，直接一键解析完成了
    if (j.contains("notes") && j["notes"].is_object()) {
        for (auto& [difficulty, notesArray] : j["notes"].items()) {
            if (notesArray.is_array()) {
                QVector<SongNoteData> noteVec;
                for (const auto& noteJson : notesArray) {
                    SongNoteData note;
                    note.from_json(noteJson);
                    noteVec.push_back(note);
                }
                difficulties.notes[QString::fromStdString(difficulty)] = noteVec;
            }
        }
    }

    // variation 不在 JSON 中，由代码填充，用于存储到Song对象的difficulty里面
}

std::optional<SongMetaData> SongDataParser::parseSongMetaData_VS(const json& j, const QString& filename) {
    semver::version current_version;
    std::string versionStr = "2.2.4";

    if (j.contains("version") && j["version"].is_string()) {
        versionStr = j["version"].get<std::string>();
    }

    auto parseResult = semver::parse(versionStr, current_version);
    if (!parseResult) {
        Exception::logVersionInvalid(QString::fromStdString(versionStr), SongDataRelative::SONG_METADATA_VERSION_RULE, filename);
        return std::nullopt;
    }

    semver::version v2_3_0, v2_1_0, v2_2_0, v2_0_0;
    semver::parse("2.2.0", v2_2_0);
    semver::parse("2.3.0", v2_3_0);
    semver::parse("2.1.0", v2_1_0);
    semver::parse("2.0.0", v2_0_0);

    // 根据版本路由
    if (current_version >= v2_2_0 && current_version < v2_3_0) {
        // 2.2.x：直接解析
        return parseMetadata_v2_2_x(j);
    }
    else if (current_version >= v2_1_0 && current_version < v2_2_0) {
        // 2.1.x：解析旧结构到迁移
        SongMetadata_v2_1_0 old;
        old.from_json(j);
        return migrate_v2_1_to_v2_2(old);
    }
    else if (current_version >= v2_0_0 && current_version < v2_1_0) {
        // 2.0.x：解析旧结构 → 迁移
        SongMetadata_v2_0_0 old;
        old.from_json(j);
        return migrate_v2_0_to_v2_2(old);
    }
    else {
        // 不支持的版本
        Exception::logVersionInvalid(QString::fromStdString(versionStr), SongDataRelative::SONG_METADATA_VERSION_RULE, filename);
        return std::nullopt;
    }
}

SongMetaData SongDataParser::parseSongMetaData_PE(const QVector<QString>& difficulties,const json& j, const QString& filename)
{
    SongMetaData meta;

    // 我们这里已经处理了版本差异，因此“理论上”可以兼容无论是新版还是旧版的格式！
    try {
        json data = j;
        // 这就是重要的差异：有的根元素是一个song，有的根什么都没有
        if (j.contains("song") && j["song"].is_object()) {
            data = j["song"];
        }

        // 基础信息
        if (data.contains("song")) meta.songName = QString::fromStdString(data["song"].get<std::string>());
        if (data.contains("artist")) meta.artist = QString::fromStdString(data["artist"].get<std::string>());
        if (data.contains("charter")) meta.charter = QString::fromStdString(data["charter"].get<std::string>());

        // PE并没有版本，但是我们还是得给他设一个以便通过检查
        meta.version = SongDataRelative::SONG_METADATA_VERSION;

        // 偏移量
        if (data.contains("offset")) {
            meta.offsets.instrumental = data["offset"].get<float>();
        }

        // BPM 变化
        if (data.contains("bpm")) {
            SongTimeChange timeChange;
            timeChange.timeStamp = 0;
            // 使用默认值，PE的这个bpm变化默认不写在json里面
            timeChange.bpm = data["bpm"].get<float>();
            meta.timeChanges.push_back(timeChange);
        }

        // loops，Psych 默认不循环
        if (data.contains("needsVoices")) {
            meta.looped = false;
        }

        // 难度
        for (auto& difficulty : difficulties)
        {
            meta.playData.difficulties.push_back(difficulty);
        }

        // 角色配置
        if (data.contains("player1")) meta.playData.characters.player = QString::fromStdString(data["player1"].get<std::string>());
        if (data.contains("player2")) meta.playData.characters.opponent = QString::fromStdString(data["player2"].get<std::string>());
        if (data.contains("gfVersion")) meta.playData.characters.girlfriend = QString::fromStdString(data["gfVersion"].get<std::string>());

        // 舞台
        if (data.contains("stage")) meta.playData.stage = QString::fromStdString(data["stage"].get<std::string>());
        // 没有的话，使用dad battle的舞台。
        else meta.playData.stage = "stage";

        // 音符样式
        if (data.contains("arrowSkin")) {
            meta.playData.noteStyle = QString::fromStdString(data["arrowSkin"].get<std::string>());
        } else {
            meta.playData.noteStyle = "funkin";
        }

        // TODO: 变体，这个东西，我目前还没见到有用上的，因此我们先默认他为空，把变体也当作普通歌曲解析。
        // 人话: 等出问题了再说
        meta.playData.songVariations = {"default"};

        meta.generatedBy = Common::DEFAULT_GENERATED_BY;

        // 时间格式
        meta.timeFormat = SongTimeFormat::MILLISECONDS;

    } catch (const std::exception& e) {
        LOG_JSON_PARSE_ERROR(false,e.what(),filename);
    }

    return meta;
}

SongChartData SongDataParser::parseSongChartData_VS(const json& j, const QString& filename) {
    SongChartData s;
    s.from_json(j);
    return s;
}

SongChartData SongDataParser::parseSongChartData_PE(const QString& difficulty,const json& j,const QString& filename)
{
    SongChartData chart;

    try {
        // 处理可能的嵌套结构
        json data = j;
        if (j.contains("song") && j["song"].is_object()) {
            data = j["song"];
        }

        chart.version = SongDataRelative::SONG_CHART_DATA_VERSION;
        chart.generatedBy = Common::DEFAULT_GENERATED_BY;

        // 解析 notes（节拍数组）
        if (data.contains("notes") && data["notes"].is_array()) {

            for (const auto& sectionJson : data["notes"]) {
                // 获取节拍属性
                bool mustHitSection = false;
                if (sectionJson.contains("mustHitSection")) {
                    mustHitSection = sectionJson["mustHitSection"].get<bool>();
                }

                // 解析 sectionNotes（音符数组）
                if (sectionJson.contains("sectionNotes") && sectionJson["sectionNotes"].is_array()) {
                    for (const auto& noteJson : sectionJson["sectionNotes"]) {
                        if (noteJson.is_array() && noteJson.size() >= 3) {
                            float time = noteJson[0].get<float>();
                            int dataValue = noteJson[1].get<int>();
                            float length = noteJson[2].get<float>();

                            // 计算最终 data（方向 + 轨道）
                            // dataValue % 4 = 方向，mustHitSection 决定轨道
                            int direction = dataValue % 4;
                            int strumline = mustHitSection ? 0 : 1;  // 玩家=0，对手=1
                            int finalData = direction + strumline * 4;

                            SongNoteData note;
                            note.time = (time);
                            note.data = finalData;
                            note.length = (length);

                            // 音符类型（如果有）
                            if (noteJson.size() >= 4) {
                                std::string typeStr = noteJson[3].get<std::string>();
                                if (!typeStr.empty()) {
                                    note.kind = QString::fromStdString(typeStr);
                                }
                            }

                            // 使用difficulties.notes来存储（为什么可以这样用，因为[]运算符会自动添键。）
                            if (difficulty == "")
                            chart.difficulties.notes[difficulty].push_back(note);
                        }
                    }
                }
            }
        }

        // 事件数组，PE还允许单独存放events.json，因此我们等会必须连那个也解析着
        if (data.contains("events") && data["events"].is_array()) {
            for (const auto& eventJson : data["events"]) {
                if (eventJson.is_array() && eventJson.size() >= 2) {
                    float time = eventJson[0].get<float>();

                    if (eventJson[1].is_array()) {
                        for (const auto& eventData : eventJson[1]) {
                            if (eventData.is_array() && eventData.size() >= 1) {
                                SongEventData event;
                                event.time = time;
                                event.eventKind = QString::fromStdString(eventData[0].get<std::string>());

                                // 这是事件的动态值，不同的事件有不同的值，数据结构不定，所以必须运行时手动解析并使用。我们这里用nlohmann表示
                                json valueJson;
                                if (eventData.size() >= 2) {
                                    valueJson["value1"] = eventData[1].get<std::string>();
                                }
                                if (eventData.size() >= 3) {
                                    valueJson["value2"] = eventData[2].get<std::string>();
                                }
                                event.value = valueJson;

                                chart.events.push_back(event);
                            }
                        }
                    }
                }
            }
        }
        // 解析单独的并合并events.json（这部分逻辑已经放到主解析函数（ModScanUtils内了！我决定最终合并成一个完整的官方引擎铺面格式后再对其进行转换。））
        /**
        for (auto& event : parseEvents_PE(jEvents,filename))
        {
            chart.events.push_back(event);
        }
        // 排序
        sort(chart.events.begin(),chart.events.end(),
            [](const SongEventData& a, const SongEventData& b) {
                    return a.time < b.time;
        });
        **/

        // 滚动速度
        float speed = 1.0;
        if (data.contains("speed")) {
            speed = data["speed"].get<float>();
        }
        chart.scrollSpeed["normal"] = (speed);

    } catch (const std::exception& e) {
        LOG_JSON_PARSE_ERROR(false,e.what(),filename);
    }

    return chart;
}

QVector<SongEventData> SongDataParser::parseEvents_PE(const json& j, const QString& filename)
{
    QVector<SongEventData> events;

    // TODO: 由于PE的某些事件命名与官方引擎的不一样，这里需要转换！等开始可玩了再转吧
    try {
        if (!j.contains("events") || !j["events"].is_array()) {
            return events;
        }

        for (const auto& eventGroup : j["events"]) {
            if (!eventGroup.is_array() || eventGroup.size() < 2) {
                continue;
            }

            // 时间戳
            float time = eventGroup[0].get<float>();

            // 事件数组
            if (!eventGroup[1].is_array()) {
                continue;
            }

            for (const auto& eventData : eventGroup[1]) {
                if (!eventData.is_array() || eventData.size() < 1) {
                    continue;
                }

                SongEventData event;
                event.time = (time);
                event.eventKind = QString::fromStdString(eventData[0].get<std::string>());

                // 打包参数
                nlohmann::json valueJson;
                if (eventData.size() >= 2) {
                    valueJson["value1"] = eventData[1].get<std::string>();
                }
                if (eventData.size() >= 3) {
                    valueJson["value2"] = eventData[2].get<std::string>();
                }
                event.value = valueJson;

                events.push_back(event);
            }
        }

    } catch (const std::exception& e) {
        LOG_JSON_PARSE_ERROR(false,e.what(),filename);
    }

    return events;
}

SongChartData SongDataParser::mergeChartData_PE(
    const QVector<SongChartData>& chartDatas,
    const QVector<SongEventData>& eventDatas)
{
    SongChartData res;

    if (chartDatas.isEmpty()) {
        LOG_INFO("传入了空的铺面文件！");
        return res;
    }

    // 用于去重的集合：记录独立事件中的 (时间戳, 事件名)
    QSet<QPair<float, QString>> externalEventKeys;
    for (const auto& event : eventDatas) {
        externalEventKeys.insert({event.time, event.eventKind});
    }

    bool eventsMerged = false;

    for (const auto& chartData : chartDatas) {
        // 1. 合并音符
        for (auto it = chartData.difficulties.notes.begin();
             it != chartData.difficulties.notes.end(); ++it) {
            const QString& diff = it.key();
            const QVector<SongNoteData>& noteList = it.value();
            res.difficulties.notes[diff].append(noteList);
        }

        // 2. 合并滚动速度
        for (auto it = chartData.scrollSpeed.begin();
             it != chartData.scrollSpeed.end(); ++it) {
            const QString& diff = it.key();
            float speed = it.value();
            res.scrollSpeed[diff] = speed;
        }

        // 3. 合并内嵌事件，这会过滤掉与独立事件冲突的
        if (!chartData.events.isEmpty()) {
            for (const auto& event : chartData.events) {
                QPair<float, QString> eventKey = {event.time, event.eventKind};
                // 如果独立事件中不存在相同的时间戳和事件名，才添加
                if (!externalEventKeys.contains(eventKey)) {
                    res.events.push_back(event);
                }
            }
            eventsMerged = true;
        }

        // 4. 元数据
        res.generatedBy = chartData.generatedBy;
        res.version = chartData.version;
    }

    // 4. 添加独立事件（优先级更高，所有独立事件都添加）
    for (const auto& event : eventDatas) {
        res.events.push_back(event);
    }

    std::sort(res.events.begin(), res.events.end(),
        [](const SongEventData& a, const SongEventData& b) {
            return a.time < b.time;
        });

    return res;
}

