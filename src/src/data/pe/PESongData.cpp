#include "data/pe/PESongData.h"

void PESection::from_json(const json& j)
{
    if (j.contains("sectionNotes") && j["sectionNotes"].is_array()) {
        sectionNotes.clear();
        for (const auto& note : j["sectionNotes"]) {
            QVector<QVariant> n;
            if (note.is_array()) {
                for (const auto& val : note) {
                    if (val.is_number()) n.push_back(val.get<double>());
                    else if (val.is_string()) n.push_back(QString::fromStdString(val.get<std::string>()));
                    else n.push_back(QVariant());
                }
            }
            sectionNotes.push_back(n);
        }
    }
    if (j.contains("sectionBeats")) sectionBeats = j["sectionBeats"].get<float>();
    if (j.contains("mustHitSection")) mustHitSection = j["mustHitSection"].get<bool>();
    if (j.contains("altAnim")) altAnim = j["altAnim"].get<bool>();
    if (j.contains("gfSection")) gfSection = j["gfSection"].get<bool>();
    if (j.contains("bpm")) bpm = j["bpm"].get<float>();
    if (j.contains("changeBPM")) changeBPM = j["changeBPM"].get<bool>();
}

void PESongFile::from_json(const json& j)
{
    // PE 歌曲可能直接在最外层或嵌套在 "song" 键下
    json songData = j;
    if (j.contains("song") && j["song"].is_object())
        songData = j["song"];

    if (songData.contains("song")) song = QString::fromStdString(songData["song"].get<std::string>());
    if (songData.contains("bpm")) bpm = songData["bpm"].get<float>();
    if (songData.contains("speed")) speed = songData["speed"].get<float>();
    if (songData.contains("offset")) offset = songData["offset"].get<float>();
    if (songData.contains("needsVoices")) needsVoices = songData["needsVoices"].get<bool>();

    if (songData.contains("player1")) player1 = QString::fromStdString(songData["player1"].get<std::string>());
    if (songData.contains("player2")) player2 = QString::fromStdString(songData["player2"].get<std::string>());
    if (songData.contains("gfVersion")) gfVersion = QString::fromStdString(songData["gfVersion"].get<std::string>());
    if (songData.contains("stage")) stage = QString::fromStdString(songData["stage"].get<std::string>());

    if (songData.contains("arrowSkin")) arrowSkin = QString::fromStdString(songData["arrowSkin"].get<std::string>());
    if (songData.contains("splashSkin")) splashSkin = QString::fromStdString(songData["splashSkin"].get<std::string>());
    if (songData.contains("disableNoteRGB")) disableNoteRGB = songData["disableNoteRGB"].get<bool>();

    if (songData.contains("gameOverChar")) gameOverChar = QString::fromStdString(songData["gameOverChar"].get<std::string>());
    if (songData.contains("gameOverSound")) gameOverSound = QString::fromStdString(songData["gameOverSound"].get<std::string>());
    if (songData.contains("gameOverLoop")) gameOverLoop = QString::fromStdString(songData["gameOverLoop"].get<std::string>());
    if (songData.contains("gameOverEnd")) gameOverEnd = QString::fromStdString(songData["gameOverEnd"].get<std::string>());

    if (songData.contains("format")) format = QString::fromStdString(songData["format"].get<std::string>());

    // 段落
    if (songData.contains("notes") && songData["notes"].is_array()) {
        notes.clear();
        for (const auto& sec : songData["notes"]) {
            PESection section;
            section.from_json(sec);
            notes.push_back(section);
        }
    }

    // 事件 — PE 事件格式: [[time, [[name, val1, val2]]]]
    if (songData.contains("events") && songData["events"].is_array()) {
        events.clear();
        for (const auto& evt : songData["events"]) {
            QVector<QVariant> eventArr;
            if (evt.is_array()) {
                for (const auto& val : evt) {
                    if (val.is_number()) eventArr.push_back(val.get<double>());
                    else if (val.is_string()) eventArr.push_back(QString::fromStdString(val.get<std::string>()));
                    else eventArr.push_back(QVariant());
                }
            }
            events.push_back(eventArr);
        }
    }
}

void PEWeekData::from_json(const json& j)
{
    if (j.contains("songs") && j["songs"].is_array()) {
        songs.clear();
        for (const auto& s : j["songs"]) {
            if (s.is_array()) {
                QVector<QVariant> entry;
                for (const auto& val : s) {
                    if (val.is_string()) entry.push_back(QString::fromStdString(val.get<std::string>()));
                    else if (val.is_array()) {
                        QVector<QVariant> sub;
                        for (const auto& v : val) sub.push_back(v.get<int>());
                        entry.push_back(QVariant::fromValue(sub));
                    }
                }
                songs.push_back(entry);
            }
        }
    }

    if (j.contains("weekCharacters") && j["weekCharacters"].is_array()) {
        weekCharacters.clear();
        for (const auto& c : j["weekCharacters"])
            weekCharacters.push_back(QString::fromStdString(c.get<std::string>()));
    }
    if (j.contains("weekBackground")) weekBackground = QString::fromStdString(j["weekBackground"].get<std::string>());
    if (j.contains("weekBefore")) weekBefore = QString::fromStdString(j["weekBefore"].get<std::string>());
    if (j.contains("storyName")) storyName = QString::fromStdString(j["storyName"].get<std::string>());
    if (j.contains("weekName")) weekName = QString::fromStdString(j["weekName"].get<std::string>());
    if (j.contains("startUnlocked")) startUnlocked = j["startUnlocked"].get<bool>();
    if (j.contains("hiddenUntilUnlocked")) hiddenUntilUnlocked = j["hiddenUntilUnlocked"].get<bool>();
    if (j.contains("hideStoryMode")) hideStoryMode = j["hideStoryMode"].get<bool>();
    if (j.contains("hideFreeplay")) hideFreeplay = j["hideFreeplay"].get<bool>();
    if (j.contains("difficulties")) difficulties = QString::fromStdString(j["difficulties"].get<std::string>());
}
