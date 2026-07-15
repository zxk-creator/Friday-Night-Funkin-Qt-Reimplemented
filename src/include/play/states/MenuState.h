//
// Created by kkplay on 7/13/26.
//

#pragma
#include "play/game/FlxSprite.h"
#include "play/game/FlxState.h"
#include "HaxeParser/type/HClass.h"

// 主菜单
class MenuState : public FlxState, public HClass
{

    QVector<QPointer<FlxSprite>> menuOptions;
public:
    MenuState(const QString& name,const QString& QMLFileAbsolutePath) : FlxState(name, QMLFileAbsolutePath), HClass("MenuState",nullptr)
    {
        registerNativeMethod("createMenuOption", FunctionType([this](const std::vector<Dynamic>& args) -> Dynamic
        {
            if (args.size() == 3 && args[0].isString() && args[1].isString() && args[2].isString() && args[3].
                isNumber())
                this->createMenuOption(args[0].asString(), args[1].asString(), args[2].asString(), args[3].asNumber());

            return Dynamic();
        }));
        
        Context::_FlxG->currentLevel = std::move(this);
    }

    void create() override
    {

    }

    void update(float elapsed) override
    {

    }

    /**
     * 记得加"/"多一个无所谓，少了完蛋
     * @param name 动画名称
     * @param relativePath 相对于可执行文件的路径
     * @param xmlPrefix 在XML里面的name，不带0000后面的这些序号
     */
    void createMenuOption(const QString& name,const QString& relativePath,const QString& xmlPrefix,int fps)
    {
        QString assetPath = Path::getAssetRoot() + relativePath;

        QPointer<FlxSprite> menuOption = new FlxSprite();
        QVector<QString> filePath;
        filePath.push_back(assetPath);
        menuOption->loadGraphic(filePath, CharacterRenderType::Sparrow);
        menuOption->animation->addByPrefix("idle",xmlPrefix, fps);
        menuOption->animation->play("idle",true);
    }

    ~MenuState() override
    {

    }


};
