import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "../../../fonts"
import "../../../buttons"

Rectangle {
    id: root
    width: parent.width
    height: gameCanvas.dp(60)
    color: "#CC000000"
    anchors.bottom: parent.bottom

    // 定义信号通知主界面切换QML
    signal sectionChanged(string qmlFile)

    RowLayout {
        anchors.fill: parent
        spacing: 0

        //左侧图标 + 文字
        DefaultButton {
            id: backButton
            text: "返回"

            normalColor: "#00CCFF"
            hoverColor: "#33DDFF"
            pressColor: "#0099CC"

            Layout.preferredWidth: gameCanvas.dp(50)
            Layout.fillHeight: true

            buttonRadius: 0

            showHoverMsg: true
            hoverMsg: qsTr("返回模组列表")
            showTextOutline: false;
            onClicked: {
                mainStack.pop()
            }
        }

        // 中间功能选项分区
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: gameCanvas.dp(25)

            // 占位符：使文字居中
            Item { Layout.fillWidth: true }

            // 管理底边栏功能
            ListModel {
                id: navModel
                ListElement { name: qsTr("总览"); file: "sectionLoader/AssetPreview.qml" }
                ListElement { name: qsTr("贴图动画"); file: "AssetsView.qml" }
                ListElement { name: qsTr("箭头皮肤"); file: "SkinView.qml" }
                ListElement { name: qsTr("歌曲试听"); file: "ListenMusic.qml" }
                ListElement { name: qsTr("脚本预览"); file: "ScriptView.qml" }
            }

            Repeater {
                model: navModel
                delegate: Text {
                    text: model.name
                    color: "white"
                    font.pixelSize: gameCanvas.dp(23)
                    Layout.alignment: Qt.AlignCenter

                    opacity: mouseItem.containsMouse ? 1.0 : 0.7

                    MouseArea {
                        id: mouseItem
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            // 触发信号让外部Loader切换文件
                            root.sectionChanged(model.file)
                        }
                    }
                }
            }
            // 同占位符：使文字居中
            Item { Layout.fillWidth: true }
        }
    }

    // 顶部的彩虹线条
    Rectangle {
        width: parent.width
        height: gameCanvas.dp(3)
        anchors.top: parent.top
        gradient: Gradient {
            orientation: Gradient.Horizontal
            GradientStop { position: 0.0; color: "#00CCFF" }
            GradientStop { position: 0.5; color: "#99FF00" }
            GradientStop { position: 1.0; color: "#FF00FF" }
        }
    }
}