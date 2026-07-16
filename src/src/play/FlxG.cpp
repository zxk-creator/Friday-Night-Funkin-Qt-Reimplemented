#include "play/FlxG.h"

void GameWindow::handleBeforeSync()
{
    int pendingKillCount = 0;
    auto now = std::chrono::steady_clock::now();
    auto deltaTime = now -lastFrameTime;
    lastFrameTime = now;

    for (auto obj : registeredGameObjects)
    {
        if (obj.isNull())
        {
            LOG_WARNING(false,"警告, 某个已注册的类无效！");
            pendingKillCount++;
            continue;
        }
        auto flxObj = qobject_cast<FlxObject*>(obj.data());
        flxObj->update(deltaTime.count());
    }

    if (pendingKillCount >= 10)
    {
        registeredGameObjects.erase(
      std::remove_if(registeredGameObjects.begin(),
                     registeredGameObjects.end(),
                     [](const QPointer<QObject>& p) { return p.isNull();
        }),
      registeredGameObjects.end()
        );
    }
}
