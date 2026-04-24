# Friday Night Funkin Qt Reimplemented

[中文文档](ZHREADME.md) | [**English Version**](README.md)

## 📌 Introduction
- A Friday Night Funkin' (FNF) reimplementation built from scratch using **Qt** and **C++**.
- This is my first open-source project, primarily created for learning C++ and the Qt framework. **Playability is not guaranteed.**
- Driven by pure interest—the project exists because I wanted to see if I could do it.
- **Strategic Pivot:** Originally started in UE5, now migrated to Qt for a more lightweight architecture.
- **Engine Name:** Quark Engine.
- Built from the ground up without relying on any existing FNF engines. Pure "hand-crafted" logic.
- *Note: The code might be a bit messy (still learning!!!).*

## 🛠 Roadmap
- [x] Implement custom mod parsing & mod selection UI
- [ ] Build basic game interface
- [ ] Develop custom JSON / XML / PNG parsing modules
- [ ] Implement mod asset loading and core gameplay logic
- [ ] Develop script compatibility layer
- [ ] Full reimplementation complete

*This is a hobby project. Estimated development time: 1–2 years.*

## ✅ How to Build
Clone the repository to your local drive and open the directory with **Visual Studio** or **CLion**. It should be ready to configure and build.

It's recommended to use **Linux** to develop this project.

## 🚀 Development Environment
<img src="./DocumentImages/Clion.png" width="60" alt="CLion">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<img src="./DocumentImages/Visual%20Studio.png" width="60" alt="VS">
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<img src="./DocumentImages/C++.png" width="60" alt="Cpp">

* **IDE**: Visual Studio 2022 / Qt Creator / **CLion (Recommended)**
* **Language**: C++17
* **Framework**: Qt 6.9.2
* **Build System**: CMake / qmake
* **OS**: Linux(Recommended)

*PS: VS Code works too, but configuration can be a headache. Best for those who enjoy troubleshooting.*

## 🤝 Contributors
- **LaoKun(myself)**

## 📁 Project Structure
Mirroring the original FNF (Official Engine) layout
* QML: Interface description files.
* src: Source code directory.
* src/include: Header files (includes some inline implementations).
* src/src: .cpp source files.
* docker: One-click build environment (Work in Progress).
* android: Java source code and manifest files for Android APK builds.
* mod: Core game art assets sourced from the official engine (does not include all playable weeks).
* main.cpp: The main entry point of the program. Start your reference here.
* rc: Windows icon and resource files.

## 📄 License
To be honest, I haven't decided if I need one yet.

## 📸 Screenshots
I guess I have one. But it's Hello World. Do you want to see?

## 💬 Contact Author
- [Youtube - LaoKun](https://www.youtube.com/@LaoKunStudio)
- zxkverygood@gmail.com

## 📜 Credits
* [Friday Night Funkin'](https://github.com/ninjamuffin99/Funkin) - Huge thanks to the original team for this amazing game.
* Thanks to the entire open-source community for the technical inspiration.