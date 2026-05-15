import QtQuick
import QtQuick.Window
import QtQuick.Controls
import "../../../../fonts"

Item {
    id: assetPreview

    property string modDetailInfo: ""

    Component.onCompleted: {
        modDetailInfo = ModRegistry.getCacheContent();
    }

    // 模组解析完成后，在这里接收信号
    Connections {
        target: ModRegistry
        // 这是QML硬性规定，与signal函数，前面必须价格on，后面首字母大写
        function onModParsed(){
            log.logInfo("收到模组解析完成信号","modParsed");
            modDetailInfo = ModRegistry.getCacheContent();
        }
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: gameCanvas.dp(40)
        color: "#AA000000"
        radius: 15
        border.color: "#33FFFFFF"
        border.width: 1

        // 左上角标题
        Mainfont {
            id: titleText
            text: "总览"
            font.pixelSize: gameCanvas.dp(60)
            color: "#ffffff"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: gameCanvas.dp(25)
        }

        // 文本内容区
        ScrollView {
            id: contentScroll
            anchors.top: titleText.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: gameCanvas.dp(25)
            clip: true

            Mainfont {
                width: contentScroll.width
                text: modDetailInfo
                color: "white"
                font.family: "Microsoft YaHei"
                font.pixelSize: gameCanvas.dp(22)
                textFormat: Text.PlainText
            }
        }
    }

    NumberAnimation {
        id: fadeInAnim
        target: assetPreview
        property: "opacity"
        from: 0
        to: 1
        duration: 200
    }
}