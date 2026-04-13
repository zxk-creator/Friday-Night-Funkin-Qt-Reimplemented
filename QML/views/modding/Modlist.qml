import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../../fonts"
import "../../buttons"

Item{
    id: root
    visible: true
    anchors.fill: parent

    Image{
        id: backgroundImage
        anchors.fill: parent
        source: "qrc:/mods/default/images/menuBG.png"
        fillMode: Image.PreserveAspectCrop
        z: -10
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 20 // 留出边缘间距
        spacing: 20        // 左右两个垂直布局之间的间距

        // 左侧垂直布局
        ColumnLayout {
            Layout.preferredWidth: 300
            Layout.fillHeight: true
            spacing: 10

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#88222222"
                Mainfont {
                    text: "模组列表"; color: "white"; anchors.centerIn: parent
                }
                radius: 10
            }

            DefaultButton {
                text: "重新加载";

            }
            DefaultButton {
                text: "禁用全部";
                normalColor: "#9EFF2121"
                pressColor: "#9EFF0000"
                hoverColor: "#F4FF1414"
            }
        }

        // 右侧垂直布局
        ColumnLayout {
            Layout.preferredWidth: 500 // 固定右侧宽度，模拟属性面板
            Layout.fillHeight: true

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 200
                color: "#AA111111"
                radius: 10

                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 10
                    spacing: 20

                    Image {
                        id: bigIcon
                        source: "qrc:/mods/default/images/icons/unknownMod.png"
                        Layout.preferredWidth: 160
                    }

                    Mainfont {
                        text: "名称未知的模组"
                        font.pixelSize: 36
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignLeft
                    }

                    // 占位符
                    Item { Layout.fillHeight: true }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#AA111111"
                radius: 10

                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 20
                    clip: true // 防止文字滚动出圆角框

                    // 滚动条样式定制（可选）
                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AsNeeded
                    }

                    ColumnLayout {
                        width: parent.width // 强制宽度等于 ScrollView 宽度以触发换行
                        spacing: 15

                        // 实际描述文本
                        Mainfont {
                            Layout.fillWidth: true
                            text: "模组作者好像忘了写概述..."
                            font.pixelSize: 30
                        }

                        // 如果还有其他信息（比如模组冲突、前置依赖）可以继续往下添加
                    }
                }
            }
        }
    }
}