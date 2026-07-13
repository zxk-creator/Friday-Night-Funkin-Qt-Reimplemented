import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// 自定义全局统一输入框
TextField {

    property string placeholder: "json绝对路径"
    property color placeholderColor: "grey"
    property color textColor: "black"
    property color borderColorNormal: "gray"
    property color borderColorFocus: "blue"
    property int borderRadius: 5
    property int borderHeight: 40

    id: input2
    width: parent.width
    placeholderText: placeholder
    placeholderTextColor: placeholderColor
    selectByMouse: true

    color: textColor
    text: ""

    background: Rectangle {
        implicitHeight: borderHeight
        radius: borderRadius
        border.color: input2.activeFocus ? borderColorFocus : borderColorNormal
        color: "white"
    }
}