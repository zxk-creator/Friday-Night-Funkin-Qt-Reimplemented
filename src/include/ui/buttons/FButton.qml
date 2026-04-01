import QtQuick 2.3
import QtQuick.Window

Rectangle {
    id: root

    // 外部可修改
    property string buttonText: "默认按钮文字"
    property color pressedColor: "#aaaaaa"
    property color normalColor: "#ffffff"

    signal clicked()

    width: 200
    height: 80
    color: mouseArea.pressed ? pressedColor : normalColor
    radius: 10

    Text {
        text: root.buttonText
        anchors.centerIn: parent
        font.pixelSize: 24
        font.bold: true
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            // 触发我们定义的信号
            root.clicked()
        }
    }
}
