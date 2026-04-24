import QtQuick
import QtQuick.Window
import QtQuick.Controls
import "../../../../fonts"

Item {
    id: assetPreview

    Rectangle {
        anchors.fill: parent
        anchors.margins: gameCanvas.dp(40) // 与边缘保持间隙
        color: "#AA000000"
        radius: 15
        border.color: "#33FFFFFF"
        border.width: 1

        // 2. 左上角标题
        Mainfont {
            id: titleText
            text: "总览"
            font.pixelSize: gameCanvas.dp(60)
            color: "#ffffff"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: gameCanvas.dp(25)
        }

        // 3. 文本内容区（带滚动条）
        ScrollView {
            id: contentScroll
            anchors.top: titleText.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: gameCanvas.dp(25)
            clip: true

            Text {
                width: contentScroll.width
                text: root.modAbsolutePath
                color: "white"
                font.family: "Microsoft YaHei"
                font.pixelSize: gameCanvas.dp(22)
                textFormat: Text.StyledText

                // 自动换行
                wrapMode: Text.WordWrap

                // 文字排版微调
                lineHeight: 1.2
                horizontalAlignment: Text.AlignLeft
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