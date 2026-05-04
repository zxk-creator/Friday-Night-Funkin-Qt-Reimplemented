import QtQuick
import QtQuick.Window
import QtQuick.Controls
import "./views"
import "views/info"

// 我们的主窗口界面！
ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1200
    height: 800
    title: "Mod Loader for - Friday Night Funkin'"
    minimumWidth: 800
    minimumHeight: 450

    // 在这里写每帧渲染以及动画逻辑（自动垂直同步）！
    FrameAnimation{

    }

    background: Rectangle { color: "black" }
    // 固定比例容器，模拟原版肥牛饭缩放操作
    Item {
        id: gameCanvas

        readonly property bool isMobile: Qt.platform.os === "android" || Qt.platform.os === "ios"

        // 左上角黑色罪恶都市风格提示框
        ViceCityInfo {
            id: globalHintMsg
            anchors.top: parent.top
            anchors.left:parent.left
            anchors.margins: 20
            z: 999

            // 全局单例，我们可以直接调用showNotification(msg)来进行信息展示
        }

        // 原始分辨率
        readonly property real logicalWidth: 1920
        readonly property real logicalHeight: 1080
        // 长宽比
        readonly property real logicalAspectRatio: logicalWidth / logicalHeight
        // 我们的缩放因子
        readonly property real scaleFactor: width / logicalWidth

        // 计算当前窗口的比例
        readonly property real windowAspectRatio: parent.width / parent.height

        // 决定画布的大小
        // 如果窗口更宽以窗口的高度为基准计算宽度。如果窗口更高以窗口的宽度为基准计算高度
        // 响应式设计，手机电脑不一样！
        width: isMobile
                   ? parent.width
                   : (windowAspectRatio > logicalAspectRatio
                       ? parent.height * logicalAspectRatio
                       : parent.width)

        height: isMobile
                    ? parent.height  // 手机直接铺满！
                    : (windowAspectRatio > logicalAspectRatio
                        ? parent.height
                        : parent.width / logicalAspectRatio) // 电脑上固定比例

        // 将画布居中
        anchors.centerIn: parent

        // 防止子元素超出画布
        clip: true

        // 调试用：显示一个红框看画布区域
        /*
        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.color: "red"
            border.width: 4
            z: 1000
        }
        */

        // 所有的游戏内容（StackView, 背景图等）都放在这里
        StackView {
            // 这些属性依旧会保留！即便是你push了
            id: mainStack
            anchors.fill: parent // 填满 gameCanvas，而不是填满 window
            // 界面内容跟随画布大小

            // 设置初始页面
            initialItem: "views/modding/Modlist.qml"
            // 压栈操作，也就是替换界面，还能返回上一界面
            // mainStack.push();
            // pop(), replace()
        }

        // 子项调用这个来布局
        function dp(value){
                return value * scaleFactor
        }
    }
}
