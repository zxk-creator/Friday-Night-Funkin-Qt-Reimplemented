import QtQuick
import QtQuick.Layouts
import "../fonts"

Item {
    id: root
    property alias text: label.text
    // 颜色属性暴露（设置默认值）
    property color normalColor: "#9E000000"   // 默认半透黑
    property color hoverColor: "#8E8E8EDD"    // 默认悬停灰
    property color pressColor: "#B0FFFFDD"    // 默认点击白
    signal clicked() // 定义点击信号

    // 没设置width时生效
    Layout.fillWidth: true
    Layout.preferredHeight: 60

    Rectangle {
        id: bg
        anchors.fill: parent
        radius: 10
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
            color: "white"
        }

        MouseArea {
            id: ma
            anchors.fill: parent
            hoverEnabled: true
            onClicked: root.clicked() // 触发根部的信号
        }
    }
}