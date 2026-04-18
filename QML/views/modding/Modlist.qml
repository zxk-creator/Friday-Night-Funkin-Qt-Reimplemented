import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "../../fonts"
import "../../buttons"
import "../info"

Item{
    id: root
    visible: true
    anchors.fill: parent

    // 使用我们建立的观察者模式数量！
    readonly property bool hasMod: ModManager.model.count > 0
    property real leftDownBtnWidth: gameCanvas.dp(650)

    // 一进入这个页面就开始扫描模组！
    Component.onCompleted: {
        console.log("页面已创建，获取已获得的模组！");
    }

    Image{
        id: backgroundImage
        anchors.fill: parent
        source: "qrc:/mods/default/images/menuBG.png"
        fillMode: Image.PreserveAspectCrop
        z: -10
    }

    // 条件判断是否有模组，显示什么界面
    Loader {
        id: modsView
        anchors.fill: parent

        sourceComponent: {
            return hasMod ? hasModLayout : noModsLayout;
        }
    }

    // 常住模组页面，让你可以方便的热加载模组
    DefaultButton {
        text: qsTr("重新加载");
        hoverMsg: qsTr("重新读取磁盘上mods目录下的内容热重载模组")
        width: root.leftDownBtnWidth
        height: gameCanvas.dp(55)
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: gameCanvas.dp(30)
        showHoverMsg: true

        z: 50

        onClicked: {
            ModManager.scanMods();
        }
    }

    // 空模组界面
    Component {
        id: noModsLayout
        EmptyModView {
            anchors.fill: parent
        }
    }

    // 正常显示的模组界面
    Component {
        id: hasModLayout
        Item {

            width: modsView.width
            height: modsView.height
            RowLayout {
                anchors.fill: parent
                anchors.margins: gameCanvas.dp(20)
                anchors.bottomMargin: gameCanvas.dp(100)
                spacing: gameCanvas.dp(20)        // 左右垂直布局之间的间距!

                // 左侧垂直布局
                ColumnLayout {
                    Layout.preferredWidth: gameCanvas.dp(300)
                    Layout.fillHeight: true
                    spacing: gameCanvas.dp(10)

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "#88222222"
                        radius: 10

                        ListView {
                            id: modsListView
                            anchors.fill: parent
                            clip: true
                            spacing: gameCanvas.dp(5)

                            // 数据绑定，响应式设计，您只需要修改这个，屏幕上模组列表就会自动改变，传入JSON对象。
                            model: ModManager.model
                            // 数据模板，定义每个条目应该长什么样？正好使用我们专门定义的ModItem.qml，数据（model）还会自动注入
                            delegate: ModItem {}
                        }
                    }
                    DefaultButton {
                        text: qsTr("禁用全部");
                        normalColor: "#9EFF2121"
                        pressColor: "#9EFF0000"
                        hoverColor: "#F4FF1414"
                        hoverMsg: qsTr("关闭所有模组（这样只有一个教程可以玩...）")
                        showHoverMsg: true
                        Component.onCompleted: root.leftDownBtnWidth
                        onWidthChanged: root.disableAllBtnWidth = width
                    }
                }

                // 右侧垂直布局
                ColumnLayout {
                    Layout.preferredWidth: gameCanvas.dp(500)
                    Layout.fillHeight: true

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: gameCanvas.dp(200)
                        color: "#AA111111"
                        radius: 10

                        // 标题栏
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: gameCanvas.dp(20)
                            spacing: gameCanvas.dp(10)

                            Image {
                                id: bigIcon
                                // 绑定我们ModItem里面被自动注入的modelData对象！他也会跟着改变
                                source: (modsListView.currentItem && modsListView.currentItem.micon)
                                    ? "file://" + modsListView.currentItem.micon
                                    : "qrc:/mods/default/images/icons/unknownMod.png"
                                Layout.preferredWidth: gameCanvas.dp(150)
                                Layout.preferredHeight: gameCanvas.dp(150)
                                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                                fillMode: Image.PreserveAspectFit
                            }

                            Mainfont {
                                text: modsListView.currentItem ? modsListView.currentItem.mtitle : qsTr("名称未知的模组")
                                font.pixelSize: gameCanvas.dp(30)
                                Layout.fillWidth: true
                                horizontalAlignment: Text.AlignLeft
                            }

                            // 占位符
                            Item {
                                Layout.fillWidth: true
                            }
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "#AA111111"
                        radius: 10

                        // 这个是为了防止内容穿过最下面的按钮
                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: gameCanvas.dp(15)
                            spacing: gameCanvas.dp(5)

                            ScrollView {
                                // TODO:这儿控制台老是报错说必须删掉这个，因为子项不能再使用这个进行布局了。但是实际上我删了他就不正常了，有待研究。
                                anchors.fill: parent
                                anchors.bottomMargin: gameCanvas.dp(45)
                                clip: true

                                ColumnLayout {
                                    width: parent.width
                                    spacing: gameCanvas.dp(15)

                                    // 实际描述文本
                                    Mainfont {
                                        Layout.fillWidth: true
                                        text: modsListView.currentItem.mdetail
                                        font.pixelSize: gameCanvas.dp(30)
                                    }
                                }
                            }
                        }
                        // 查看详情按钮
                        DefaultButton {
                            id: detailButton
                            height: gameCanvas.dp(40)
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                            anchors.margins: gameCanvas.dp(15)

                            Layout.fillWidth: true
                            text: qsTr("查看模组详情！")
                            hoverMsg: qsTr("查看此模组的详细信息，比如角色贴图，箭头皮肤，歌曲等等。")
                            showHoverMsg: true
                            pressMsg: qsTr("暂未开放，敬请期待！")
                            showPressMsg: true
                        }
                    }
                }
            }
        }
    }
}