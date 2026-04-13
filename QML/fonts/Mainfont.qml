// Mainfont.qml
import QtQuick

Item {
    id: root

    //把外部想控制的属性全部接通到内部Text
    property alias text: internalText.text
    property alias horizontalAlignment: internalText.horizontalAlignment
    property alias verticalAlignment: internalText.verticalAlignment
    property alias font: internalText.font
    property alias color: internalText.color

    // 让Item的大小自动跟随内部文字大小
    implicitWidth: internalText.implicitWidth
    implicitHeight: internalText.implicitHeight

    FontLoader {
        id: customFont
        source: "qrc:/mods/default/fonts/GenshinFont.ttf"
    }

    Text {
        id: internalText
        // 填满外层 Item
        anchors.fill: parent

        // 默认设置
        font.family: customFont.name
        font.pixelSize: 30
        color: "#ffffff"
        renderType: Text.NativeRendering

        style: Text.Outline
        styleColor: "#000000"
    }
}