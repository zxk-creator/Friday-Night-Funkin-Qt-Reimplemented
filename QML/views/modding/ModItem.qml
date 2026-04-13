// ModDelegate.qml
import QtQuick
import QtQuick.Layouts
import "../fonts"

Rectangle {
    id: delegateRoot
    width: ListView.view.width
    height: 80
    radius: 8

    // 模拟选中状态
    property bool isSelected: ListView.isCurrentItem
    property color normalColor: "#9E000000"   // 默认半透黑
    property color hoverColor: "#8E8E8EDD"    // 默认悬停灰
    property color pressColor: "#B0FFFFDD"    // 默认点击白
    property color selectedColor: "#55FFFFFF"

    // 判断模组选择框颜色的函数
    function determineColor() {
        if (isSelected) return selectedColor;
        if (ma.containsPress) return pressColor;
        if (ma.containsMouse) return hoverColor;

        return normalColor;
    }

    color: determineColor()

    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 15

        // 模组图标
        Image {
            source: model.icon // 绑定数据中的 icon 路径
            Layout.preferredWidth: 60
            Layout.preferredHeight: 60
            fillMode: Image.PreserveAspectFit
        }

        // 模组名字
        Mainfont {
            text: model.name // 绑定数据中的 name
            Layout.fillWidth: true
            font.pixelSize: 18
            horizontalAlignment: Text.AlignLeft
        }
    }

    MouseArea {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            delegateRoot.ListView.view.currentIndex = index // 切换当前选中项
            // 这里可以触发读取详情的逻辑
        }
    }
}