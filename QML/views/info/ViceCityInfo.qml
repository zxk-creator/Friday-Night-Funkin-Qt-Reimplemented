import QtQuick
import QtQuick.Controls
import "../../fonts"

// 罪恶都市风提示信息框！
Rectangle {
    id: infoBox

    property alias displayMsg: label.text
    width: label.implicitWidth + 10  // 根据文字宽度自动调节
    height: label.height + 20
    color: "#CC000000"
    border.width: 1

    opacity: 0                      // 默认不可见
    visible: opacity > 0

    // 文字内容
    Mainfont {
        id: label
        anchors.centerIn: parent
        color: "white"
        font.pixelSize: gameCanvas.dp(20)

        horizontalAlignment: Text.AlignHCenter
    }

    SequentialAnimation on opacity {
        id: fadeAnimation
        running: false

        // 瞬间出现，不依赖动画而是直接设置透明度
        // 保持显示状态1.5秒
        PauseAnimation { duration: 4000 }

        // 0.5 秒淡出消失
        NumberAnimation {
            to: 0
            duration: 1500
            easing.type: Easing.OutCubic
        }
    }

    // 外部调用实现动画播放
    function showNotification(msg) {
        fadeAnimation.stop();      // 如果当前正在播放动画，先停止
        infoBox.displayMsg = msg;
        infoBox.opacity = 1.0;     // 瞬间显示
        fadeAnimation.start();     // 开始！
        SoundSystem.playinfoSound();
    }
}