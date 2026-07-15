#include "play/game/FlxSprite.h"

#include <QRegularExpression>
#include <QMap>

#define PUGIXML_HEADER_ONLY
#include "pugixml/pugixml.hpp"

QVector<FlxAnimation> FlxSprite::parseSparrow(const QString& pngAbsolutePath)
{
    QVector<FlxAnimation> result;

    // 找到对应的 XML
    QString xmlPath;
    if (pngAbsolutePath.endsWith(".png", Qt::CaseInsensitive))
        xmlPath = pngAbsolutePath.left(pngAbsolutePath.length() - 4) + ".xml";
    else
        xmlPath = pngAbsolutePath + ".xml";

    // 解析 XML
    pugi::xml_document doc;
    if (!doc.load_file(xmlPath.toStdString().c_str()))
    {
        LOG_WARNING(false,"无法解析Sparrow格式图集");
        return result;
    }

    // 取第一张已加载纹理
    QSGTexture* sourceTex = nullptr;
    for (auto* t : altas)
    {
        if (t) { sourceTex = t; break; }
    }

    // 按前缀分组
    QMap<QString, QVector<FlxFrame>> frameGroups;

    static QRegularExpression instanceRegex(R"(^(.*)\s+instance\s+\d+$)");
    static QRegularExpression digitSuffixRegex(R"(^(.*?)(\d+)$)");

    pugi::xml_node atlasNode = doc.child("TextureAtlas");
    for (pugi::xml_node sub : atlasNode.children("SubTexture"))
    {
        QString name = QString::fromStdString(sub.attribute("name").as_string());
        if (name.isEmpty()) continue;

        // 提取前缀（动画名）
        QString prefix;
        auto match = instanceRegex.match(name);
        if (match.hasMatch())
            prefix = match.captured(1).trimmed();
        else
        {
            match = digitSuffixRegex.match(name);
            prefix = match.hasMatch() ? match.captured(1) : name;
        }
        while (prefix.endsWith(' ') || prefix.endsWith('_') || prefix.endsWith('-'))
            prefix.chop(1);
        if (prefix.isEmpty()) prefix = name;

        // 帧数据
        FlxRect rect;
        rect.x = sub.attribute("x").as_int(0);
        rect.y = sub.attribute("y").as_int(0);
        rect.w = sub.attribute("width").as_int(0);
        rect.h = sub.attribute("height").as_int(0);

        int fX = sub.attribute("frameX").as_int(0);
        int fY = sub.attribute("frameY").as_int(0);
        int fW = sub.attribute("frameWidth").as_int(0);
        int fH = sub.attribute("frameHeight").as_int(0);

        frameGroups[prefix].emplaceBack(
            sourceTex, name, rect,
            sub.attribute("flippedX").as_bool(false),
            sub.attribute("flippedY").as_bool(false),
            fX, fY, fW, fH);
    }

    // 排序 + 构造动画
    static QRegularExpression numRe(R"((\d+)$)");
    for (auto it = frameGroups.begin(); it != frameGroups.end(); ++it)
    {
        auto& frames = it.value();
        std::sort(frames.begin(), frames.end(), [](const FlxFrame& a, const FlxFrame& b) {
            auto ma = numRe.match(a.name);
            auto mb = numRe.match(b.name);
            return (ma.hasMatch() ? ma.captured(1).toInt() : 0)
                 < (mb.hasMatch() ? mb.captured(1).toInt() : 0);
        });

        FlxAnimation anim;
        anim.name = it.key();
        anim.frames = std::move(frames);
        result.push_back(std::move(anim));
    }

    animations = result;
    return result;
}

void FlxSprite::loadGraphic(QVector<QString>& imageAbsolutePaths, CharacterRenderType renderType)
{
    int validImgCount = 0;
    // 循环的目的是为了兼容多个图集。如果只有一个图集只需要传入一个即可。
    for (const auto& path : imageAbsolutePaths)
    {
        if (!QFileInfo::exists(path)) continue;
        QSGTexture* tex = window()->createTextureFromImage(QImage(path));
        if (tex == nullptr) continue;
        validImgCount++;

        switch (renderType)
        {
        case CharacterRenderType::AnimateAtlas:
        case CharacterRenderType::MultiAnimateAtlas:
        case CharacterRenderType::Packer:
        case CharacterRenderType::Sparrow:
            {
                altas.push_back(tex);
                animations = parseSparrow(path);
                break;
            }
        case CharacterRenderType::Custom:
        case CharacterRenderType::MultiSparrow:
            break;
        default: break;
        }
    }
    if (validImgCount > 0) setFlag(ItemHasContents, true);

   QQuickItem::update();
}

void FlxSprite::update(float elapsed)
{


    if (!animation)
    {
        animation->update(elapsed);
    }

    QQuickItem::update();
}

FlxSprite::FlxSprite(QQuickItem* parent) : QQuickItem(parent), HClass("FlxSprite",nullptr)
{
    animation = std::make_shared<FlxAnimationController>(this);
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(true);
    setFlag(ItemHasContents);
}

QSGNode* FlxSprite::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data)
{
    {
        if (altas.isEmpty() || animations.isEmpty() || !currentTex) return nullptr;

        QSGGeometryNode* node = static_cast<QSGGeometryNode*>(oldNode);
        QSGOpaqueTextureMaterial* material = nullptr;

        if (!node)
        {
            // 首次：创建节点 + 几何 + 材质
            node = new QSGGeometryNode();

            auto* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
            node->setGeometry(geometry);
            node->setFlag(QSGNode::OwnsGeometry);

            material = new QSGOpaqueTextureMaterial();
            material->setTexture(currentTex);
            node->setMaterial(material);
            node->setFlag(QSGNode::OwnsMaterial);
        }
        else
        {
            material = static_cast<QSGOpaqueTextureMaterial*>(node->material());
        }

        // 纹理切换了？（多图集角色）
        if (material && material->texture() != currentTex)
            material->setTexture(currentTex);

        // 计算 UV 坐标（像素 → 0~1 归一化）
        QSize texSize = currentTex->textureSize();
        qreal texW = static_cast<qreal>(texSize.width());
        qreal texH = static_cast<qreal>(texSize.height());

        qreal srcX = static_cast<qreal>(currentSampingPoint.x) / texW;
        qreal srcY = static_cast<qreal>(currentSampingPoint.y) / texH;
        qreal srcW = static_cast<qreal>(currentSampingPoint.w) / texW;
        qreal srcH = static_cast<qreal>(currentSampingPoint.h) / texH;

        QSGGeometry::updateTexturedRectGeometry(
            node->geometry(),
            QRectF(0, 0, currentSampingPoint.w, currentSampingPoint.h),
            QRectF(srcX, srcY, srcW, srcH)
        );

        node->markDirty(QSGNode::DirtyMaterial);
        return node;
    }
}


int FlxAnimationController::addByPrefix(const QString& animName, const QString& xmlPrefix, int fps, bool looped)
{
    if (!spriteOwner || spriteOwner->animations.isEmpty()) return 0;

    QVector<FlxFrame> found;

    // 1) 精确匹配 FlxAnimation::name == xmlPrefix
    for (const auto& anim : spriteOwner->animations)
    {
        if (anim.name == xmlPrefix)
        {
            found = anim.frames;
            break;
        }
    }

    // 2) 回退：FlxAnimation::name 包含 xmlPrefix
    if (found.isEmpty())
    {
        for (const auto& anim : spriteOwner->animations)
        {
            if (anim.name.contains(xmlPrefix, Qt::CaseInsensitive))
            {
                found = anim.frames;
                break;
            }
        }
    }

    // 3) 回退：逐个帧名做 startsWith 匹配
    if (found.isEmpty())
    {
        for (const auto& anim : spriteOwner->animations)
        {
            for (const auto& f : anim.frames)
            {
                if (f.name.startsWith(xmlPrefix, Qt::CaseInsensitive))
                    found.push_back(f);
            }
            if (!found.isEmpty()) break;
        }

        if (!found.isEmpty())
        {
            static QRegularExpression numRe(R"((\d+)$)");
            std::sort(found.begin(), found.end(), [](const FlxFrame& a, const FlxFrame& b) {
                auto ma = numRe.match(a.name);
                auto mb = numRe.match(b.name);
                return (ma.hasMatch() ? ma.captured(1).toInt() : 0)
                     < (mb.hasMatch() ? mb.captured(1).toInt() : 0);
            });
        }
    }

    if (found.isEmpty()) return 0;

    AnimEntry entry;
    entry.name = animName;
    entry.frames = std::move(found);
    entry.fps = fps;
    entry.looped = looped;
    int count = entry.frames.size();
    anims[animName] = std::move(entry);
    return count;
}
