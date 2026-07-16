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
    SharedPtr<class FlxAnimationController> animation;
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

    void create() override;

    void update(float elapsed) override;

    void kill() override {

    }

    Dynamic getField(const QString& fieldName) override;
    QString getName() override;

protected:
    void hoverEnterEvent(QHoverEvent* event) override;
    void hoverMoveEvent(QHoverEvent* event) override;
    void hoverLeaveEvent(QHoverEvent* event) override;
};




// 动画控制器
class FlxAnimationController : public HClass
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
    explicit FlxAnimationController(FlxSprite* owner);

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
    void update(float deltaTime);

    /**
     * 播放指定动画
     */
    void play(const QString& animName, bool reset = true);

    // 获取当前帧持续多少秒
    float getCurrentFrameDuration();

    // 切换到下一帧
    void switchToNextFrame();
};
