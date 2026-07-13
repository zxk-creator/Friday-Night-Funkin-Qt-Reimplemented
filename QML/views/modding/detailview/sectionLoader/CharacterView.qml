import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../../../../custom/inputbox"
import "../../../../buttons"

Item {
    id: view

    property string imageX: "0"
    property string imageY: "0"
    property string imageW: "0"
    property string imageH: "0"
    property string imagePath: ""
    property real scale: 1.0
    property string currentAnim: "idle"
    property int currentOffset_X: 0
    property int currentOffset_Y: 0
    property var jsonObject: null
    property bool imageLoading: false
    property bool imageLoaded: false

    // ==================== 左侧控制面板 ====================
    Rectangle {
        id: controlPanel
        width: 300
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 10
        color: "#2c3e50"
        radius: 8

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 15
            spacing: 12

            // 标题
            Text {
                text: "图片裁剪工具"
                color: "white"
                font.pixelSize: 18
                font.bold: true
                Layout.alignment: Qt.AlignHCenter
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#445566"
            }

            // 图片路径
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5

                Text {
                    text: "图片路径"
                    color: "#ecf0f1"
                    font.pixelSize: 12
                }

                FNFInputField {
                    id: imagePathInput
                    Layout.fillWidth: true
                    height: 38
                    placeholder: "/path/to/image.png"
                    placeholderColor: "#95a5a6"
                    textColor: "black"
                    borderColorNormal: "#7f8c8d"
                    borderColorFocus: "#3498db"
                    borderRadius: 4
                    borderHeight: 38
                }
            }

            // 裁剪参数 - 两行两列
            Text {
                text: "裁剪参数"
                color: "#ecf0f1"
                font.pixelSize: 12
                Layout.topMargin: 5
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        Text { text: "X:"; color: "#ecf0f1"; Layout.preferredWidth: 20 }
                        FNFInputField {
                            id: xInput
                            Layout.fillWidth: true
                            height: 32
                            text: "0"
                            placeholder: "0"
                            borderHeight: 32
                            borderRadius: 4
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        Text { text: "Y:"; color: "#ecf0f1"; Layout.preferredWidth: 20 }
                        FNFInputField {
                            id: yInput
                            Layout.fillWidth: true
                            height: 32
                            text: "0"
                            placeholder: "0"
                            borderHeight: 32
                            borderRadius: 4
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        Text { text: "W:"; color: "#ecf0f1"; Layout.preferredWidth: 20 }
                        FNFInputField {
                            id: wInput
                            Layout.fillWidth: true
                            height: 32
                            text: "100"
                            placeholder: "100"
                            borderHeight: 32
                            borderRadius: 4
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 5
                        Text { text: "H:"; color: "#ecf0f1"; Layout.preferredWidth: 20 }
                        FNFInputField {
                            id: hInput
                            Layout.fillWidth: true
                            height: 32
                            text: "100"
                            placeholder: "100"
                            borderHeight: 32
                            borderRadius: 4
                        }
                    }
                }
            }

            // 缩放
            RowLayout {
                Layout.fillWidth: true
                spacing: 10

                Text { text: "缩放:"; color: "#ecf0f1"; Layout.preferredWidth: 40 }
                FNFInputField {
                    id: scaleInput
                    Layout.fillWidth: true
                    height: 32
                    text: "1.0"
                    placeholder: "1.0"
                    borderHeight: 32
                    borderRadius: 4
                }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 1
                color: "#445566"
            }

            // 按钮
            DefaultButton {
                id: parseButton
                text: "加载并绘制"
                Layout.fillWidth: true
                height: 40
                onClicked: {
                    var rawPath = imagePathInput.text.trim()
                    if (rawPath === "") return

                    var imagePath = rawPath
                    if (!rawPath.startsWith("file://")) {
                        imagePath = "file://" + rawPath
                    }

                    view.imagePath = imagePath
                    view.imageX = xInput.text.trim() || "0"
                    view.imageY = yInput.text.trim() || "0"
                    view.imageW = wInput.text.trim() || "100"
                    view.imageH = hInput.text.trim() || "100"
                    view.scale = parseFloat(scaleInput.text.trim()) || 1.0
                    view.imageLoaded = false
                    view.imageLoading = true

                    thisCanvas.loadImage(view.imagePath)
                }
            }

            Item { Layout.fillHeight: true }
        }
    }

    // ==================== 右侧画布 ====================
    Rectangle {
        anchors.left: controlPanel.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 10
        color: "#1a1a2e"
        radius: 8
        border.color: "#16213e"
        border.width: 1

        Canvas {
            id: thisCanvas
            anchors.fill: parent
            anchors.margins: 1

            onImageLoaded: {
                view.imageLoaded = true
                view.imageLoading = false
                requestPaint()
            }

            onPaint: {
                var ctx = getContext("2d")
                ctx.clearRect(0, 0, width, height)

                if (view.imagePath === "") {
                    ctx.fillStyle = "#ffffff"
                    ctx.font = "20px sans-serif"
                    ctx.textAlign = "center"
                    ctx.fillText("请在上方输入图片路径", width/2, height/2)
                    return
                }

                if (view.imageLoading || !view.imageLoaded) {
                    ctx.fillStyle = "#ffffff"
                    ctx.font = "20px sans-serif"
                    ctx.textAlign = "center"
                    ctx.fillText("加载图片中...", width/2, height/2)
                    return
                }

                if (!thisCanvas.isImageLoaded(view.imagePath)) {
                    ctx.fillStyle = "#ffffff"
                    ctx.font = "20px sans-serif"
                    ctx.textAlign = "center"
                    ctx.fillText("图片加载失败", width/2, height/2)
                    return
                }

                var cropX = parseFloat(view.imageX) || 0
                var cropY = parseFloat(view.imageY) || 0
                var cropW = parseFloat(view.imageW) || 0
                var cropH = parseFloat(view.imageH) || 0

                if (cropW <= 0 || cropH <= 0) {
                    ctx.fillStyle = "#ff6b6b"
                    ctx.font = "16px sans-serif"
                    ctx.textAlign = "center"
                    ctx.fillText("宽度和高度必须大于0", width/2, height/2)
                    return
                }

                var currentScale = view.scale > 0 ? view.scale : 1.0
                var scaledW = cropW * currentScale
                var scaledH = cropH * currentScale
                var centerX = (width - scaledW) / 2
                var centerY = (height - scaledH) / 2

                ctx.drawImage(
                    view.imagePath,
                    cropX, cropY, cropW, cropH,
                    centerX, centerY,
                    scaledW, scaledH
                )

                ctx.strokeStyle = "#00ff88"
                ctx.lineWidth = 2
                ctx.strokeRect(centerX, centerY, scaledW, scaledH)
            }
        }
    }

    // ==================== 底部动作面板 ====================
    Rectangle {
        anchors.bottom: parent.bottom
        anchors.left: controlPanel.right
        anchors.right: parent.right
        anchors.margins: 10
        height: 100
        color: "#2c3e50"
        radius: 8

        Grid {
            anchors.centerIn: parent
            columns: 9
            spacing: 6

            component AnimButton : Button {
                property string animName: ""
                property int offsetX: 0
                property int offsetY: 0
                text: animName
                font.pixelSize: 11
                implicitWidth: 65
                implicitHeight: 32
                background: Rectangle {
                    color: parent.pressed ? "#3498db" : (parent.hovered ? "#2980b9" : "#34495e")
                    radius: 4
                }
                contentItem: Text {
                    text: parent.text
                    color: "white"
                    font: parent.font
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                onClicked: {
                    view.currentAnim = animName
                    view.currentOffset_X = offsetX
                    view.currentOffset_Y = offsetY
                    thisCanvas.requestPaint()
                }
            }

            AnimButton { animName: "idle";          offsetX: 7;  offsetY: 0 }
            AnimButton { animName: "singLEFT";      offsetX: 0;  offsetY: 0 }
            AnimButton { animName: "singLEFTmiss";  offsetX: 0;  offsetY: 0 }
            AnimButton { animName: "singUP";        offsetX: 0;  offsetY: 0 }
            AnimButton { animName: "singUPmiss";    offsetX: 0;  offsetY: 0 }
            AnimButton { animName: "singRIGHT";     offsetX: 0;  offsetY: 0 }
            AnimButton { animName: "singRIGHTmiss"; offsetX: 0;  offsetY: 0 }
            AnimButton { animName: "singDOWN";      offsetX: 0;  offsetY: 0 }
            AnimButton { animName: "singDOWNmiss";  offsetX: 0;  offsetY: 0 }
        }
    }

    // ==================== 状态信息 ====================
    Rectangle {
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 10
        width: 160
        height: 70
        color: "#000000"
        opacity: 0.7
        radius: 5
        z: 10

        Column {
            anchors.centerIn: parent
            spacing: 3
            Text {
                text: "状态: " + (view.imageLoading ? "加载中" : (view.imageLoaded ? "已加载" : "未加载"))
                color: view.imageLoaded ? "#00ff88" : "#ff6b6b"
                font.pixelSize: 11
            }
            Text {
                text: "缩放: " + view.scale.toFixed(2)
                color: "white"
                font.pixelSize: 10
            }
            Text {
                text: "裁剪: " + view.imageW + "x" + view.imageH
                color: "white"
                font.pixelSize: 10
            }
        }
    }
}