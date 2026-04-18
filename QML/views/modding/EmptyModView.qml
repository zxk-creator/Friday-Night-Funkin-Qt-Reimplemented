import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../../fonts"
import "../../buttons"

Item {
    id: rootItem
    anchors.fill: parent

    Mainfont {
        id: noModText
        anchors.centerIn: parent
        text: qsTr("未找到任何一个mod！")
        font.bold: true

        SequentialAnimation on opacity {
            id: breatheAnimation
            loops: Animation.Infinite // 无限循环
            running: true             // 自动开始

            // 淡入
            NumberAnimation {
                from: 0.0
                to: 1.0
                duration: 1000
                easing.type: Easing.InOutQuad // 使用平滑的缓动曲线
            }

            // 淡出
            NumberAnimation {
                from: 1.0
                to: 0.0
                duration: 1000
                easing.type: Easing.InOutQuad
            }
        }
    }

    /*
    DefaultButton {
        id: exitButton
        text: qsTr("退出")

        width: 150
        height: 60

        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 20 // 设置边距，防止紧贴边缘

        // TODO:退出交互逻辑，由于没有主界面，因此先打印一个log
        onClicked: {
            console.log("退出选择模组")
        }
    }
    */
}