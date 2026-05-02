# Quark Engine 基于 Qt Framework

[**中文文档**](ZHREADME.md) | [English Version](README.md)

## 简介：
- 这是一个基于Qt做的从零开始复刻FNF的项目，也是我的第一个开源项目，使用C++ 开发。
- 主要目的是为了学习C++和Qt，并不保证能玩。
- 兴趣驱动学习，所以就有了这个项目。
- 原本使用UE5开发，最后战略转型。
- 准备定名为 **Quark Engine**
- 不基于任何原版引擎，从零开始纯手搓！
- *代码写的有点水。*

## 🛠 实现路线：
- [x] 手搓模组解析并构建选择模组界面
- [ ] 手搓基础游戏界面
- [ ] 手搓json/xml/png解析模块
- [ ] 手搓解析模组文件并开始可玩
- [ ] 手搓兼容脚本
- [ ] 复刻成功

*业余时间开发，前前后后预计需要一到两年的时间。*

## ✅ 如何构建：
克隆本项目后随便拷贝到磁盘的某个位置用VS或者CLion打开项目下的CMakeList.txt即可。需要先根据Qt文档下载依赖库搭建开发环境。
后续将会使用dockerfile一键编译。

## 🚀 开发环境
<img src="./DocumentImages/Clion.png" width="50" alt="CLion">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<img src="./DocumentImages/Visual%20Studio.png" width="50" alt="CLion">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<img src="./DocumentImages/C++.png" width="50" alt="Cpp">

* **IDE**: Visual Studio 2022 / Qt Creator / CLion（推荐）
* **语言**: C++17
* **框架**: Qt 6.9.2
* **构建系统**: CMake / qmake
* **操作系统**: 建议使用Linux

*PS:VSCode也行，但是配置麻烦。适合喜欢折腾的玩家*

## 🤝 贡献者：
**Laokun**

## 📁 项目结构
**类似原版FNF（官方引擎）**
* QML: 界面描述文件
* src: 源代码文件
* src/include: 头文件，包含部分实现
* src/src: .cpp源代码文件
* docker: 一键拉取后即可编译（尚未实现）
* android: 构建安卓apk时相关的Java代码和清单文件
* mod: 游戏运行的基本美术资源，取自官方引擎（不包含全部可玩周目）
* main.cpp，程序运行主入口。**你可以从这里开始参考。**
* rc: windows图标文件。

## 📄 许可证
GPL

## 📸 截图
* 模组选择界面
<img src="./DocumentImages/ScreenShots/ModSelectScreen.png" width="1000" alt="ModSelect">

## 📚使用的库
* [miniaudio/stbvorbis](https://miniaud.io/): 负责底层音频播放
* [nlohmann](https://json.nlohmann.me/): json文件解析
* [pugixml](https://pugixml.org/): xml文件解析
* [semver](https://semver.org/): 字符串版本规则解析

## 💬 联系作者
https://space.bilibili.com/533393738?spm_id_from=333.1007.0.0

## 📜 鸣谢
* [Friday Night Funkin'](https://github.com/ninjamuffin99/Funkin) - 感谢原作者团队带来的伟大作品。
* [Psych Engine](https://github.com/ShadowMario/FNF-PsychEngine/releases) - 源码参考与设计模式
* 以及所有开源社区提供的技术灵感。