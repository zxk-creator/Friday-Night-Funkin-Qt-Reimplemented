// Mainfont.qml
import QtQuick
import fnf

Item {
    id: root

    //把外部想控制的属性全部接通到内部Text
    property alias text: internalText.text
    property alias horizontalAlignment: internalText.horizontalAlignment
    property alias verticalAlignment: internalText.verticalAlignment
    property alias font: internalText.font
    property alias color: internalText.color
    property bool showTextOutline: true

    // 让Item的大小自动跟随内部文字大小
    implicitWidth: internalText.implicitWidth
    implicitHeight: internalText.implicitHeight

    FontLoader {
        id: customFont
        source: PathUtil.font("GenshinFont")
    }

    Text {
        id: internalText
        // 填满外层 Item
        anchors.fill: parent

        // 支持html风语法！
        textFormat: Text.StyledText

        onLinkActivated: (link) => {
            Qt.openUrlExternally(link)
        }

        wrapMode: Text.Wrap

        // 默认设置
        font.family: customFont.name
        font.pixelSize: gameCanvas.dp(30)
        color: "#ffffff"
        renderType: Text.NativeRendering

        style: root.showTextOutline ? Text.Outline : Text.Normal
        styleColor: "#000000"
    }
}