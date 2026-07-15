//
// Created by kkplay on 7/12/26.
//

#pragma once
#include <QFileInfo>
#include "FlxObject.h"
#include <QQuickItem>
#include <qquickwindow.h>
#include <QSGFlatColorMaterial>
#include <QSGOpaqueTextureMaterial>
#include <QSGTexture>

#include "data/Context.h"
#include "data/character/CharacterData.h"
#include "HaxeParser/type/HClass.h"

class FlxSprite;
class FlxFrame;
class FlxAnimation;

using ControllerPtr = std::shared_ptr<class FlxAnimationController>;

struct FlxRect
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;
};






// 代表了一张大的纹理图集中一个小帧
class FlxFrame
{
public:
    QSGTexture* sourceTex = nullptr;
    QString name;
    // 渲染所需所有坐标
    FlxRect texPosition;
    int offsetX = 0;
    int offsetY = 0;
    // 逻辑大小，和碰撞有关，暂时用不到
    int frameWidth = 0;
    int frameHeight = 0;
    // 水平翻转
    bool flipX = false;
    // 垂直翻转
    bool flipY = false;

    FlxFrame() = default;

    FlxFrame(QSGTexture* tex, const QString& nm, const FlxRect& rect,
             bool fx = false, bool fy = false,
             int ox = 0, int oy = 0, int fw = 0, int fh = 0)
        : sourceTex(tex), name(nm), texPosition(rect),
          offsetX(ox), offsetY(oy), frameWidth(fw), frameHeight(fh),
          flipX(fx), flipY(fy) {}
};






// 代表了一组FlxFrame组成的动画
class FlxAnimation
{
public:
    QString name;
    QVector<FlxFrame> frames;
};





// 代表了场景中一个独立的精灵对象
class FlxSprite : public QQuickItem, public FlxObject, public HClass
{
    Q_OBJECT
    Q_INTERFACES(FlxObject)

    friend class FlxAnimationController;

public:
    QVector<QSGTexture*> altas;
    QSGTexture* currentTex = nullptr;
    ControllerPtr animation;
    QVector<FlxAnimation> animations;
    FlxRect currentSampingPoint;

    FlxSprite(QQuickItem* parent = nullptr);

    // 相当于每帧进行的渲染
    QSGNode* updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData* data) override;

    /**
     * 解析 Sparrow 格式的精灵图集 XML 文件
     *
     *   <TextureAtlas imagePath="xx.png">
     *     <SubTexture name="BF idle dance0000" x="0" y="0" width="100" height="150"
     *                 frameX="-5" frameY="-10" frameWidth="110" frameHeight="170" />
     *     ...
     *   </TextureAtlas>
     *
     * 帧命名规则：
     *   模式1: "BF idle dance0000" → 前缀 "BF idle dance", 帧号 0000
     *   模式2: "lose... instance 10000" → 前缀 "lose...", 帧号 10000
     *
     * 返回值按帧前缀自动分组，每组就是一个 FlxAnimation。
     *
     * @param pngAbsolutePath PNG 图片的绝对路径，同目录下必须有同名.xml文件！
     * @return 解析并分组后的所有动画
     */
    QVector<FlxAnimation> parseSparrow(const QString& pngAbsolutePath);

    // 外部调用这个来加载纹理图集
    void loadGraphic(QVector<QString>& imageAbsolutePaths, CharacterRenderType renderType);

    void create() override
    {

    }

    void update(float elapsed) override;

    Dynamic getField(const QString& fieldName) override
    {
        if (fieldName == "animation")
        {
            return animation.get();
        }

        return Dynamic();
    }

protected:
    void hoverEnterEvent(QHoverEvent* event) override
    {

        QQuickItem::hoverEnterEvent(event);
    }

    void hoverMoveEvent(QHoverEvent* event) override
    {

        QQuickItem::hoverMoveEvent(event);
    }

    void hoverLeaveEvent(QHoverEvent* event) override
    {

        QQuickItem::hoverLeaveEvent(event);
    }
};




// 动画控制器
class FlxAnimationController : HClass
{
    FlxSprite* spriteOwner;

    // 已注册的播放动画
    struct AnimEntry {
        QString name;
        QVector<FlxFrame> frames;
        int fps = 24;
        bool looped = false;
        float elapsed = 0.0f;
        bool finished = false;
    };
    QHash<QString, AnimEntry> anims;
    QPair<QString, AnimEntry> currentAnim;

    float frameTimer = 0;
    int currentFrameIdx = 0;

public:
    explicit FlxAnimationController(FlxSprite* owner) : HClass("FlxAnimationController",nullptr), spriteOwner(owner)
    {
        registerNativeMethod("addByPrefix", FunctionType([this](const std::vector<Dynamic>& args) -> Dynamic
        {
            if (args.size() == 3 && args[0].isString() && args[1].isString() && args[2].isNumber() && args[3].isBool())
            {
                this->addByPrefix(args[0].asString(), args[1].asString(), args[2].asNumber(), args[3].asBool());
                return Dynamic();
            }

            LOG_ERROR(false,"您传入的函数参数不正确！");
            return Dynamic();
        }));
    }

    /**
     * 从已解析的图集中，按XML帧名前缀注册一条播放动画
     *
     * 例：parseSparrow后产生FlxAnimation{ name="BF idle dance", frames=[...] }
     *     addByPrefix("idle", "BF idle dance", 24) → 注册为 "idle"，播放时 24fps
     *
     * @param animName   播放名称（如"idle", "singLEFT"）
     * @param xmlPrefix  XML 帧名完整前缀（"BF idle dance"），与parseSparrow输出的FlxAnimation::name 进行匹配
     * @param fps        帧率
     * @param looped     是否循环
     * @return 找到的帧数，0 = 失败
     */
    int addByPrefix(const QString& animName, const QString& xmlPrefix, int fps = 24, bool looped = true);

    // 请在FlxSprite的beforeSynchronizing里面调用这个。这是模拟的每帧循环
    void update(float deltaTime)
    {
        if (currentAnim.second.frames.isEmpty() || currentAnim.second.finished)
            return;

        frameTimer += deltaTime;

        if (frameTimer >= getCurrentFrameDuration())
        {
            switchToNextFrame();
            frameTimer = 0;
        }
    }

    /**
     * 播放指定动画
     */
    void play(const QString& animName, bool reset = true)
    {
        auto it = anims.find(animName);
        if (it == anims.end())
        {
            LOG_WARNING(false, "未找到动画" + animName);
            return;
        }

        currentAnim.first = it.key();
        currentAnim.second = it.value();
        frameTimer = 0;
        if (currentAnim.second.frames.size() <= 0) return;

        spriteOwner->currentTex = currentAnim.second.frames[0].sourceTex;
        if (reset) currentFrameIdx = 0;
    }

    // 获取当前帧持续多少秒
    float getCurrentFrameDuration()
    {
        int fps = currentAnim.second.fps;
        if (fps <= 0) fps = 24;
        return 1.0f / static_cast<float>(fps);
    }

    // 切换到下一帧
    void switchToNextFrame()
    {
        int totalFrames = currentAnim.second.frames.size();
        if (totalFrames == 0)
        {
            LOG_WARNING(false, "当前动画" + currentAnim.second.name + "没有帧！");
            return;
        }

        // 到末尾了？循环回去，否则前进
        if (currentFrameIdx + 1 >= totalFrames)
        {
            currentFrameIdx = 0;
            if (!currentAnim.second.looped)
                currentAnim.second.finished = true;
        }
        else
        {
            currentFrameIdx++;
        }

        const FlxFrame& curFrame = currentAnim.second.frames[currentFrameIdx];
        spriteOwner->currentSampingPoint = curFrame.texPosition;
        spriteOwner->currentTex = curFrame.sourceTex;
    }
};
