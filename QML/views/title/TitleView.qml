import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

// 主窗口: 使用原始图片中的配色方案 (深色背景，亮色文字，红/金色点缀)
Item {
    id: titleInterface
    visible: true
    width: 800
    height: 600

    // 配色方案 (从图片中提取的主要色调)
    readonly property color backgroundColor: "#1a1a2e"      // 深蓝黑背景
    readonly property color panelColor: "#16213e"           // 深色面板
    readonly property color accentRed: "#e94560"            // 红色点缀 (类似图片中按钮高亮)
    readonly property color accentGold: "#f5a623"           // 金色 (用于边框/文字)
    readonly property color textColor: "#e0e0e0"            // 主文本色
    readonly property color buttonColor: "#0f3460"          // 按钮背景色
    readonly property color buttonHoverColor: "#e94560"      // 悬停时的红色
    readonly property color buttonPressColor: "#c73e54"      // 按下时加深的红色

    // 背景: 使用指定的图片 (假设图片资源名为 "background.png", 需放置在项目目录)
    // 由于没有实际图片文件，这里使用渐变模拟图片风格，但实际项目应使用 Image 并加载真实图片
    // 为了完全符合需求，使用 Image 组件加载 "background.jpg" 或 "background.png"
    // 请将图片命名为 "game_bg.jpg" 并放置在 qml 文件同目录或资源文件中
    Image {
        id: backgroundImage
        anchors.fill: parent
        source: "qrc:/images/background.jpg"  // 替换为实际图片路径
        fillMode: Image.PreserveAspectCrop
        // 如果不希望使用真实图片，可改用下面的矩形渐变作为后备
        visible: true

        // 半透明遮罩层，使文字更清晰 (模仿图片中的暗色覆盖层)
        Rectangle {
            anchors.fill: parent
            color: "#80000000"  // 半透明黑色
            visible: true
        }
    }

    // 后备背景 (如果没有找到图片，则显示渐变背景)
    Rectangle {
        id: fallbackBackground
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "#0a0a1a" }
            GradientStop { position: 0.5; color: "#1a1a2e" }
            GradientStop { position: 1.0; color: "#16213e" }
        }
        visible: backgroundImage.status !== Image.Ready
    }

    // 主要界面布局 - 居中显示菜单
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 20
        width: parent.width * 0.4
        Layout.alignment: Qt.AlignHCenter

        // 标题区域 (可能包含图片logo，由于没有具体图片，用文本模拟)
        Item {
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredHeight: 120
            Layout.preferredWidth: parent.width

            // 模拟标题图片 (实际应替换为图片)
            Text {
                anchors.centerIn: parent
                text: "人型自走发电机"
                font.pixelSize: 36
                font.bold: true
                font.family: "Microsoft YaHei, SimHei, sans-serif"
                color: accentGold
                style: Text.Outline
                styleColor: "#80000000"
            }

            // 版本号标签，模仿图片右下角的小字
            Text {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                text: "ver0.26c"
                font.pixelSize: 12
                color: "#aaaaaa"
                opacity: 0.8
            }
        }

        // 菜单按钮组
        ColumnLayout {
            spacing: 15
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: 200

            // 开始游戏按钮
            MenuButton {
                buttonText: "开始游戏"
                buttonColor: mainWindow.buttonColor
                textColor: mainWindow.textColor
                hoverColor: mainWindow.buttonHoverColor
                pressColor: mainWindow.buttonPressColor
            }

            // 继续游戏按钮
            MenuButton {
                buttonText: "继续游戏"
                buttonColor: mainWindow.buttonColor
                textColor: mainWindow.textColor
                hoverColor: mainWindow.buttonHoverColor
                pressColor: mainWindow.buttonPressColor
            }

            // 设置按钮
            MenuButton {
                buttonText: "设置"
                buttonColor: mainWindow.buttonColor
                textColor: mainWindow.textColor
                hoverColor: mainWindow.buttonHoverColor
                pressColor: mainWindow.buttonPressColor
            }

            // 结束按钮
            MenuButton {
                buttonText: "结束"
                buttonColor: mainWindow.buttonColor
                textColor: mainWindow.textColor
                hoverColor: mainWindow.buttonHoverColor
                pressColor: mainWindow.buttonPressColor
            }
        }

        // 底部控制提示栏 (模仿图片中的 WASD 切换确认等)
        Rectangle {
            Layout.topMargin: 40
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 40
            color: "#40000000"  // 半透明黑底
            radius: 8

            RowLayout {
                anchors.centerIn: parent
                spacing: 20

                KeyHint { hintText: "A" }
                KeyHint { hintText: "S" }
                KeyHint { hintText: "W" }
                KeyHint { hintText: "D" }

                Rectangle {
                    width: 2
                    height: 20
                    color: "#888888"
                    opacity: 0.5
                }

                KeyHint { hintText: "切换" }
                KeyHint { hintText: "确认" }
            }
        }

        // 底部版权与语言选择栏 (模仿图片底部的多语言和版权)
        Rectangle {
            Layout.topMargin: 20
            Layout.preferredWidth: parent.width
            Layout.preferredHeight: 50
            color: "#30000000"
            radius: 5

            RowLayout {
                anchors.fill: parent
                anchors.margins: 5
                spacing: 10

                // 版权文本
                Text {
                    text: "Copyright (c) 2020- NanameHacha"
                    font.pixelSize: 10
                    color: "#aaaaaa"
                    Layout.alignment: Qt.AlignLeft
                    Layout.fillWidth: true
                }

                // 作者标注
                Text {
                    text: "@hinayuar18 & @HashinoMizuha"
                    font.pixelSize: 10
                    color: "#aaaaaa"
                    Layout.alignment: Qt.AlignLeft
                }

                // 时间显示 (模拟 00:01/14:30)
                Text {
                    text: "00:01/14:30"
                    font.pixelSize: 10
                    color: accentGold
                    Layout.alignment: Qt.AlignRight
                }

                // 语言选择下拉框样式 (用按钮模拟)
                Rectangle {
                    width: 100
                    height: 25
                    color: "#30000000"
                    radius: 4
                    border.color: "#888888"
                    border.width: 1

                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 4

                        Text {
                            text: "中文(CN)"
                            font.pixelSize: 10
                            color: "#dddddd"
                        }
                        Text {
                            text: "▼"
                            font.pixelSize: 8
                            color: "#dddddd"
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            console.log("语言选择")
                            // 语言切换逻辑预留
                        }
                    }
                }

                // 小提示：English/Thai/JP 用简化的文本显示
                Text {
                    text: "English  Thai  日本語"
                    font.pixelSize: 9
                    color: "#aaaaaa"
                    Layout.alignment: Qt.AlignRight
                }
            }
        }
    }

    // 自定义按钮组件
    component MenuButton: Rectangle {
        property string buttonText: "按钮"
        property color buttonColor: "#0f3460"
        property color textColor: "#e0e0e0"
        property color hoverColor: "#e94560"
        property color pressColor: "#c73e54"

        width: 200
        height: 45
        radius: 8
        color: buttonColor
        border.width: 1
        border.color: Qt.darker(buttonColor, 1.2)

        Text {
            anchors.centerIn: parent
            text: buttonText
            font.pixelSize: 18
            font.bold: true
            color: parent.textColor
            font.family: "Microsoft YaHei, sans-serif"
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor

            onEntered: parent.color = hoverColor
            onExited: parent.color = buttonColor
            onPressed: parent.color = pressColor
            onReleased: parent.color = hoverColor
        }

        // 添加一点光晕效果
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#20ffffff" }
                GradientStop { position: 0.5; color: "#00ffffff" }
                GradientStop { position: 1.0; color: "#10ffffff" }
            }
            visible: parent.color === parent.hoverColor
        }
    }

    // 按键提示组件
    component KeyHint: Rectangle {
        property string hintText: "A"

        width: 40
        height: 30
        radius: 6
        color: "#60000000"
        border.color: "#cccccc"
        border.width: 1

        Text {
            anchors.centerIn: parent
            text: hintText
            font.pixelSize: 14
            font.bold: true
            color: "#f0f0f0"
            font.family: "Consolas, monospace"
        }
    }
}