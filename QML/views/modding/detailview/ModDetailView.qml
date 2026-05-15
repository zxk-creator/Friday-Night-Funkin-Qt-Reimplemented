import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../../../fonts"
import "../../../buttons"
import "./sectionLoader"

Item {
    id: root
    visible: true
    anchors.fill: parent

    // 接收传过来的路径！我们这里使用字符串，代表模组文件的路径
    property string modAbsolutePath;

    Component.onCompleted: {
        ModRegistry.parseOneMod(modAbsolutePath)
    }

    Image{
        id: backgroundImage
        anchors.fill: parent
        cache: true
        source: PathUtil.image("menuBG")
        fillMode: Image.PreserveAspectCrop
        z: -10
    }

    // 动态加载子页面
    Loader {
        id: contentLoader
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            bottom: bottomBar.top
        }

        // 加载时的过渡动画
        Behavior on opacity { NumberAnimation { duration: 150 } }

        source: "sectionLoader/AssetPreview.qml" // 默认页面

        onLoaded: {
            console.log("已加载模块:", source)
        }
    }

    BottomBar {
        id: bottomBar
        anchors.bottom: parent.bottom
        width: parent.width
        height: gameCanvas.dp(70)

        // 监听底边栏发出的切换信号
        onSectionChanged: (qmlFile) => {
            if (timer.targetFile === qmlFile) return;
            contentLoader.opacity = 0
            timer.targetFile = qmlFile;
            timer.start();
        }
    }

    // 我们地动画
    Timer {
        id: timer
        // 默认值，防止进来点一下就播放动画了
        property string targetFile: "sectionLoader/AssetPreview.qml"
        interval: 150
        onTriggered: {
            contentLoader.source = targetFile
            contentLoader.opacity = 1
        }
    }
}