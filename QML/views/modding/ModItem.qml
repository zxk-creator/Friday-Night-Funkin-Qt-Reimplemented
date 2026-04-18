// modlist里面每一个模组条目内容
import QtQuick
import QtQuick.Layouts
import "../../fonts"

Rectangle {
    id: delegateRoot
    width: ListView.view.width
    height: 80
    radius: 8

    // 模拟选中状态，这里的ListView是当作为ListView的附加对象的时候，QML引擎动态注入的
    property bool isSelected: ListView.isCurrentItem
    property color normalColor: "#88222222"   // 默认半透黑
    property color hoverColor: "#8E8E8EDD"    // 默认悬停灰
    property color pressColor: "#B0FFFFDD"    // 默认点击白
    property color selectedColor: "#55FFFFFF"

    // 暴露注入的model变量，防止外部访问的时候说找不到属性
    readonly property string mtitle: model.title
    readonly property string micon: model.icon
    readonly property string mdetail: model.detail

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
            // linux和windows不一样这个file
            function getfileUrl(absolutePath){
                if (!absolutePath) return "qrc:/mods/default/images/icons/unknownMod.png";
                if (Qt.platform.os === "windows"){
                    return "file:///" + absolutePath
                }
                else return "file://" + absolutePath;
            }

            // 绑定数据中的 icon 路径（绑定的是我们在Modlist.qml里面设置的那个model，字段名随便起，只要名字一样就行，需要是绝对路径）
            source: getfileUrl(model.icon)
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 60
            Layout.preferredHeight: 60
            fillMode: Image.PreserveAspectFit
            mipmap: true
        }

        // 模组名字
        Mainfont {
            text: model.title
            Layout.alignment: Qt.AlignVCenter
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
            if (!isSelected){
                SoundSystem.playconfirmSound()
            }
            delegateRoot.ListView.view.currentIndex = index // 设置当前项被选中，提供视觉反馈以及上面的isSelected设置为true！
        }

        onEntered: {
            if (!isSelected){
                SoundSystem.playscrollSound()
            }
        }
    }
}
