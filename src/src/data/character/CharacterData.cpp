//
// Created by kkplay on 5/2/26.
//

#include "data/character/CharacterData.h"

#include "Constants.h"
#include "utils/Path.h"
#include "utils/exception/CustomException.h"
#include "utils/file/FileUtil.h"
#include "utils/message/MessageHandler.h"
using namespace std;

void HealthIconData::from_json(const json& j) {
    if (j.contains("id")) id = QString::fromStdString(j["id"].get<std::string>());
    if (j.contains("scale")) scale = j["scale"].get<float>();
    if (j.contains("flipX")) flipX = j["flipX"].get<bool>();
    if (j.contains("isPixel")) isPixel = j["isPixel"].get<bool>();
    if (j.contains("offsets") && j["offsets"].is_array()) {
        QVector<float> offs;
        for (const auto& val : j["offsets"]) {
            offs.push_back(val.get<float>());
        }
        offsets = offs;
    }
}

void DeathData::from_json(const json& j) {
    if (j.contains("cameraOffsets") && j["cameraOffsets"].is_array()) {
        QVector<float> offs;
        for (const auto& val : j["cameraOffsets"]) {
            offs.push_back(val.get<float>());
        }
        cameraOffsets = offs;
    }
    if (j.contains("cameraZoom")) cameraZoom = j["cameraZoom"].get<float>();
    if (j.contains("preTransitionDelay")) preTransitionDelay = j["preTransitionDelay"].get<float>();
}

CharacterRenderType parseRenderType(const QString& str) {
    if (str == "sparrow") return CharacterRenderType::Sparrow;
    if (str == "packer") return CharacterRenderType::Packer;
    if (str == "multisparrow") return CharacterRenderType::MultiSparrow;
    if (str == "animateatlas") return CharacterRenderType::AnimateAtlas;
    if (str == "multianimateatlas") return CharacterRenderType::MultiAnimateAtlas;
    if (str == "custom") return CharacterRenderType::Custom;
    return CharacterRenderType::Sparrow;
}

void CharacterData::from_json(const json& j) {
    if (j.contains("version")) version = QString::fromStdString(j["version"].get<std::string>());
    if (j.contains("name")) name = QString::fromStdString(j["name"].get<std::string>());
    if (j.contains("renderType")) renderType = parseRenderType(QString::fromStdString(j["renderType"].get<std::string>()));
    if (j.contains("assetPath")) assetPath = QString::fromStdString(j["assetPath"].get<std::string>());
    if (j.contains("scale")) scale = j["scale"].get<float>();
    if (j.contains("healthIcon")) {
        healthIcon = HealthIconData();
        healthIcon.from_json(j["healthIcon"]);
    }
    if (j.contains("death")) {
        death = DeathData();
        death.from_json(j["death"]);
    }
    if (j.contains("offsets") && j["offsets"].is_array()) {
        QVector<float> offs;
        for (const auto& val : j["offsets"]) {
            offs.push_back(val.get<float>());
        }
        offsets = offs;
    }
    // 没事，如果版本差异导致不包含也没问题，使用默认值
    if (j.contains("cameraOffsets") && j["cameraOffsets"].is_array()) {
        QVector<float> offs;
        for (const auto& val : j["cameraOffsets"]) {
            offs.push_back(val.get<float>());
        }
        cameraOffsets = offs;
    }
    if (j.contains("isPixel")) isPixel = j["isPixel"].get<bool>();
    if (j.contains("danceEvery")) danceEvery = j["danceEvery"].get<float>();
    if (j.contains("singTime")) singTime = j["singTime"].get<float>();
    if (j.contains("startingAnimation")) startingAnimation = QString::fromStdString(j["startingAnimation"].get<std::string>());
    if (j.contains("flipX")) flipX = j["flipX"].get<bool>();
    if (j.contains("applyStageMatrix")) applyStageMatrix = j["applyStageMatrix"].get<bool>();
    if (j.contains("atlasSettings")) {
        atlasSettings = TextureAtlasData();
        atlasSettings.from_json(j["atlasSettings"]);
    }

    // 解析动画数组
    if (j.contains("animations") && j["animations"].is_array()) {
        animations.clear();
        for (const auto& animJson : j["animations"]) {
            AnimationData anim;
            anim.from_json(animJson);
            animations.push_back(anim);
        }
    }
}

std::unique_ptr<CharacterData> CharacterDataParser::parseCharacterData_VS(const json& j, const QString& filepath)
{
    try {
        auto data = std::make_unique<CharacterData>();
        data->from_json(j);

        // 验证必填字段
        if (data->assetPath.isEmpty()) {
            MessageHandler::logWarning(false,
                filepath + "的assetPath字段为空！已跳过",
                "CharacterParser"
            );
            return nullptr;
        }

        if (data->animations.isEmpty()) {
            MessageHandler::logWarning(false,
                filepath + "的animations字段为空！已跳过",
                "CharacterParser"
            );
            return nullptr;
        }

        return data;
    } catch (const std::exception& e) {
        MessageHandler::logError(true,
            "解析模组" + filepath + "失败，原因: \n" + e.what() + "\n",
            "CharacterParser"
        );
        return nullptr;
    }
}

std::unique_ptr<CharacterData> CharacterDataParser::parseCharacterData_PE(const json& j, const QString& filepath,const QString& modAbsolutePath)
{
    // shit，无论是Vs还是Pe都支持这在AnimateAtlas格式的图片，那这可难受了，做游戏的人是爽了，造轮子的可难受了。
    // 如果是普通TextureAtlas格式的图片，只需要一张图片和一个xml文件即可，"image"填入共有名字，
    // 如果是AnimateAtlas格式图片，"image"填入Animation.json,spritemap.json/png所在路径。
    auto data = std::make_unique<CharacterData>();
    try {
        // 基础信息
        if (j.contains("scale")) {
            data->scale = j["scale"].get<float>();
        } else {
            data->scale = 1.0f;
        }

        if (j.contains("sing_duration")) {
            data->singTime = j["sing_duration"].get<float>();
        } else {
            data->singTime = 4.0f;  // Psych 默认值
        }

        if (j.contains("flip_x")) {
            data->flipX = j["flip_x"].get<bool>();
        } else {
            data->flipX = false;
        }

        if (j.contains("no_antialiasing")) {
            data->isPixel = j["no_antialiasing"].get<bool>();
        } else {
            data->isPixel = false;
        }

        // 位置
        if (j.contains("position") && j["position"].is_array() && j["position"].size() >= 2) {
            data->offsets = QVector<float>();
            data->offsets.push_back(j["position"][0].get<float>());
            data->offsets.push_back(j["position"][1].get<float>());
        } else {
            data->offsets = QVector<float>{0, 0};
        }

        // 相机偏移
        if (j.contains("camera_position") && j["camera_position"].is_array() && j["camera_position"].size() >= 2) {
            data->cameraOffsets = QVector<float>();
            data->cameraOffsets.push_back(j["camera_position"][0].get<float>());
            data->cameraOffsets.push_back(j["camera_position"][1].get<float>());
        } else {
            data->cameraOffsets = QVector<float>{0, 0};
        }

        // 血条颜色
        if (j.contains("healthbar_colors") && j["healthbar_colors"].is_array() && j["healthbar_colors"].size() >= 3) {
            int r = j["healthbar_colors"][0].get<int>();
            int g = j["healthbar_colors"][1].get<int>();
            int b = j["healthbar_colors"][2].get<int>();
            QColor color = QColor(r,g,b);
            data->healthIcon.healthIconColor = color;
        }

        // 健康图标
        if (j.contains("healthicon")) {
            data->healthIcon = HealthIconData();
            data->healthIcon.id = QString::fromStdString(j["healthicon"].get<std::string>());
            data->healthIcon.scale = 1.0f;
            data->healthIcon.flipX = false;
            data->healthIcon.isPixel = false;
            data->healthIcon.offsets = QVector<float>{0, 25};
        } else {
            data->healthIcon = HealthIconData();
            data->healthIcon.id = QString::fromStdString(j["image"].get<std::string>());
        }

        // TODO: 这这地方的代码写的有点水，需要优化（回头再说）！
        // 动画（无论是什么类型的动画图集，这里的格式都一样），我们需要在这里设置Path！
        if (j.contains("animations") && j["animations"].is_array()) {
            for (const auto& animJson : j["animations"]) {
                AnimationData anim;

                if (animJson.contains("anim")) {
                    anim.name = QString::fromStdString(animJson["anim"].get<std::string>());
                }
                if (animJson.contains("name")) {
                    anim.prefix = QString::fromStdString(animJson["name"].get<std::string>());
                }
                if (animJson.contains("fps")) {
                    anim.frameRate = animJson["fps"].get<int>();
                } else {
                    anim.frameRate = 24;
                }
                if (animJson.contains("loop")) {
                    anim.looped = animJson["loop"].get<bool>();
                } else {
                    anim.looped = false;
                }
                if (animJson.contains("offsets") && animJson["offsets"].is_array() && animJson["offsets"].size() >= 2) {
                    anim.offsets = std::vector<float>();
                    anim.offsets.push_back(animJson["offsets"][0].get<float>());
                    anim.offsets.push_back(animJson["offsets"][1].get<float>());
                } else {
                    anim.offsets = std::vector<float>{0, 0};
                }
                if (animJson.contains("indices") && animJson["indices"].is_array()) {
                    anim.frameIndices = std::vector<int>();
                    for (const auto& idx : animJson["indices"]) {
                        anim.frameIndices.push_back(idx.get<int>());
                    }
                }

                // 渲染类型（根据是否有 Animation.json 判断）
                // 重中之重！PE的multiSparrow使用按','分隔的字符串，而VS是直接是一个animation一个json对象
                // 所以我们需要进行判断！
                // 不管是不是多图集，都会用到，后续animations数组里面需要遍历循环获取。
                QStringList multiSparrowAssetPaths;

                if (j.contains("image")) {
                    QString assetPathTemp = QString::fromStdString(j["image"].get<std::string>());
                    // 首先去除多余的空格
                    assetPathTemp.remove(' ');
                    // 再进行字符串分隔（如果没有,就会返回自身，正确。不过名字可能具有误导性）
                    multiSparrowAssetPaths = assetPathTemp.split(',',Qt::SkipEmptyParts);
                    // 注意这里: 是MultiSparrow还是MultiAnimateAtlas？
                    if (multiSparrowAssetPaths.size() != 1)
                    {
                        // 怎么验证: 直接去每个路径下找有没有对应的文件
                        auto res = CharacterParseUtils::detectCharacterRenderType(multiSparrowAssetPaths,modAbsolutePath,anim.name);
                        if (!res.has_value())
                        {
                            LOG_JSON_PARSE_ERROR(true,"解析JSON时出现问题", modAbsolutePath+ assetPathTemp);
                        }
                    }
                    else
                    {
                        // 走到这说明肯定不是Multi格式了
                        // 简单判断：如果有动画JSON则用AnimateAtlas，否则用Sparrow
                        // 指定动画目录
                        QString soCalledAnimFileOrDir = QDir::cleanPath(modAbsolutePath + "/images/" + assetPathTemp);
                        auto res = CharacterParseUtils::isAnimateAtlas(soCalledAnimFileOrDir);
                        if (!res.has_value()) return nullptr;

                        // 是AnimateAtlas格式
                        if (res.value())
                            data->renderType = CharacterRenderType::AnimateAtlas;
                        // 是Sparrow格式
                        else
                            data->renderType = CharacterRenderType::Sparrow;
                    }
                } else {
                    // 默认单图集，可是如果他连图片都没有那还解析什么呢
                    return nullptr;
                }

                // 到这里，我们必须亲自打开xml或者json来验证了
                // 由于官方引擎架构中，一个AnimationData需要绑定一个assetPath，所以我们必须去亲自验证
                // 这里简单验证一下，只获取json的name，跑到xml里面进行字符串查找即可。
                switch (data->renderType)
                {
                    // 单个的
                case CharacterRenderType::AnimateAtlas:
                    {
                        if (multiSparrowAssetPaths.isEmpty())
                        {
                            LOG_WARNING(false,"动画图集路径为空，这不应该发生！");
                        }
                        else
                        {
                            anim.assetPath = multiSparrowAssetPaths[0];
                        }
                        break;
                    }
                case CharacterRenderType::MultiAnimateAtlas:
                    {
                        for (auto& assetPath: multiSparrowAssetPaths)
                        {
                            QString JSONContent = FileUtil::ReadFileToString(QDir::cleanPath(modAbsolutePath + "/images/" + assetPath + "/Animation.json"));
                            int pos = JSONContent.indexOf(anim.name);
                            if (pos == -1)
                            {
                                LOG_WARNING(false,assetPath + "所处的目录并没有找到JSON中定义的动画name，已跳过");
                                continue;
                            }
                            // 找到了！直接记录资产路径
                            anim.assetPath = assetPath;
                        }
                        break;
                    }
                    // 也是单个的
                case CharacterRenderType::Sparrow:
                    {
                        // 那么这个就无需遍历，直接赋值就行
                        if (multiSparrowAssetPaths.isEmpty())
                        {
                            LOG_WARNING(false,"纹理图集路径为空，这不应该发生！");
                            // 即便是出问题了也尝试赋值吧，到时候找不到了再说
                        }
                        else
                        {
                            anim.assetPath = multiSparrowAssetPaths[0];
                        }
                        break;
                    }
                    // 多个（最复杂）
                case CharacterRenderType::MultiSparrow:
                    {
                        for (auto& assetPath: multiSparrowAssetPaths)
                        {
                            // 对于Sparrow，我们只能从XML中提取字符进行匹配
                            QString XMLContent = FileUtil::ReadFileToString(QDir::cleanPath(modAbsolutePath + "/images/" + assetPath) + ".xml");
                            int pos = XMLContent.indexOf(anim.name);
                            if (pos == -1)
                            {
                                LOG_WARNING(false,assetPath + "所处的目录并没有找到XML中定义的动画name，已跳过");
                                continue;
                            }
                            // 找到了！直接记录资产路径
                            anim.assetPath = assetPath;
                        }
                        break;
                    }
                }

                data->animations.push_back(anim);
            }
        }

        // 起始动画
        data->startingAnimation = "idle";

        // 版本
        data->version = CharacterDataRelative::CHARACTER_DATA_VERSION;

        // 名称（从文件名提取，或使用默认）
        data->name = QFileInfo(filepath).baseName();

    } catch (const std::exception& e) {
        LOG_JSON_PARSE_ERROR(false,e.what(),filepath);
        return nullptr;
    }
    return data;
}

std::optional<CharacterRenderType> CharacterParseUtils::detectCharacterRenderType(
    const QStringList& assetPaths,
    const QString& modAbsolutePath,
    const QString& animName)
{
    if (assetPaths.isEmpty()) {
        LOG_WARNING(false, "资产路径列表为空，无法检测渲染类型，这不应该发生！");
        return std::nullopt;
    }

    // 用于记录检测到的类型
    std::optional<CharacterRenderType> detectedType;
    int validCount = 0;

    for (const QString& assetPath : assetPaths) {
        QString cleanAssetPath = QDir::cleanPath(assetPath);
        QString fullPath = QDir::cleanPath(modAbsolutePath + "/images/" + cleanAssetPath);

        // 检查是否是 AnimateAtlas 格式（目录 + Animation.json）
        QFileInfo atlasFile(fullPath + "/Animation.json");

        if (atlasFile.isFile()) {
            // 是 AnimateAtlas 格式，检查是否包含目标动画
            QString jsonContent = FileUtil::ReadFileToString(atlasFile.absoluteFilePath());
            if (jsonContent.indexOf(animName) != -1) {
                if (!detectedType.has_value()) {
                    detectedType = CharacterRenderType::AnimateAtlas;
                }
                validCount++;
            }
            continue;
        }

        // 检查是否是 Sparrow 格式（.xml 文件存在即可，因为 png 是必须的但可能路径不同）
        QFileInfo xmlFile(fullPath + ".xml");

        if (xmlFile.isFile()) {
            // 是 Sparrow 格式，检查 XML 中是否包含目标动画
            QString xmlContent = FileUtil::ReadFileToString(xmlFile.absoluteFilePath());
            if (xmlContent.indexOf(animName) != -1) {
                if (!detectedType.has_value()) {
                    detectedType = CharacterRenderType::Sparrow;
                }
                validCount++;
            }
            continue;
        }

        // 无法识别的格式
        LOG_WARNING(false, QString("无法识别的资产路径: %1，已跳过").arg(assetPath));
    }

    // 根据检测结果返回
    if (validCount == 0) {
        LOG_WARNING(false, QString("在资产路径中未找到动画: %1").arg(animName));
        return std::nullopt;
    }

    // 只有一个有效资产
    if (validCount == 1) {
        return detectedType.value();
    }

    // 多个有效资产，转换为 Multi 版本
    if (detectedType.value() == CharacterRenderType::AnimateAtlas) {
        return CharacterRenderType::MultiAnimateAtlas;
    } else if (detectedType.value() == CharacterRenderType::Sparrow) {
        return CharacterRenderType::MultiSparrow;
    }

    LOG_ERROR(true,"未知原因导致没有正常返回数据，可能是由于模组的动画格式为不兼容格式，比如Packer，也可能是文件格式不符合标准！");
    return std::nullopt;
}