import QtQuick
import QtQuick.Layouts
import "../fonts"
import "../views/info"

Item {
    id: root
    property alias text: label.text
    // 颜色属性暴露（设置默认值）
    property color normalColor: "#9E000000"   // 默认半透黑
    property color hoverColor: "#8E8E8EDD"    // 默认悬停灰
    property color pressColor: "#B0FFFFDD"    // 默认点击白
    property bool showHoverMsg: false;
    property bool showPressMsg: false;
    property string hoverMsg: ""
    property string pressMsg: ""
    property int buttonRadius: 10;
    property bool showTextOutline: true
    signal clicked() // 定义点击信号

    // 没设置width时生效
    Layout.fillWidth: true
    Layout.preferredHeight: gameCanvas.dp(60)

    Rectangle {
        id: bg
        anchors.fill: parent
        radius: buttonRadius
        color: ma.containsPress
            ? root.pressColor          // 点击时
            : (ma.containsMouse
                ? root.hoverColor         // 悬停时
                : root.normalColor)        // 正常
        // 渐变动画
        // Behavior on color { ColorAnimation { duration: 150 } }

        Mainfont {
            id: label
            anchors.centerIn: parent
            showTextOutline: root.showTextOutline
            color: "white"
        }

        MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                SoundSystem.playconfirmSound()
                if (showPressMsg)
                {
                    globalHintMsg.showNotification(pressMsg);
                }
                root.clicked()
            } // 触发根部的信号
            onEntered: {
                SoundSystem.playscrollSound()
            if (showHoverMsg)
            {
                globalHintMsg.showNotification(hoverMsg);
            }
            }
        }
    }
}