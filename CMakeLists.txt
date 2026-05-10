cmake_minimum_required(VERSION 3.16)
project("FNF" LANGUAGES CXX)
set(CPACK_PACKAGE_NAME "Quark Engine")

include(qt.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Core Qml Quick Network Widgets)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Quick
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    resources.rc
        src/include/audio/FunkinSound.h
        src/include/audio/FunkinSoundSystem.h
        src/include/audio/SoundEnum.h
        src/include/save/Save.h
        src/include/save/SaveSystem.h
        src/include/utils/GlobalSystemUtils.h
        src/include/utils/Math.h
        src/include/utils/Path.h
        src/src/audio/FunkinSoundSystem.cpp
        src/src/audio/MiniaudioOggImpl.cpp
        src/src/utils/Path.cpp
        src/include/data/mod/ModMetadata.h
        src/include/modding/ModListModel.h
        src/include/utils/modding/ModScanUtils.h
        src/src/utils/modding/ModScanUtils.cpp
        src/include/utils/lang/LangStringPool.h
        src/src/utils/modding/GlobalSystemUtils.cpp
        src/src/modding/modmetadata/ModMetadata.cpp
        src/include/permissions/GetAndroidStoragePermission.h
        src/include/data/BaseRegistry.h
        src/include/modding/modmetadata/VersonRule.h
        src/thirdParty/semver/semver.h
        src/include/utils/VersionUtil.h
        src/include/data/IRegistrableEntry.h
        src/include/data/level/LevelRegistry.h
        src/include/data/level/LevelData.h
        src/include/modding/ModEngineType.h
        src/include/data/animation/AnimationData.h
        src/include/data/stage/StageData.h
        src/include/utils/exception/CustomException.h
        src/src/utils/exception/CustomException.cpp
        src/src/data/level/LevelData.cpp
        src/include/utils/file/FileUtil.h
        src/include/data/song/SongRegistry.h
        src/include/play/song/Song.h
        src/include/data/song/SongData.h
        src/include/Constants.h
        src/src/modding/modmetadata/VersonRule.cpp
        src/src/permissions/GetAndroidStoragePermission.cpp
        src/src/data/song/SongData.cpp
        src/include/utils/message/MessageHandler.h
        src/include/utils/interseting/InterestingThings.h
        src/include/play/ISerializable.h
        src/include/utils/extension/ExtendJSONParse.h
        src/include/data/character/CharacterData.h
        src/src/data/character/CharacterData.cpp
        src/src/utils/file/FileUtil.cpp
        src/include/data/character/CharacterRegistry.h
        src/include/data/notestyle/NoteStyleData.h
        src/src/data/notestyle/NoteStyleData.cpp
        src/include/data/mod/ModRegistry.h
        src/src/data/mod/ModRegistry.cpp
        src/src/modding/ModListModel.cpp
        src/include/data/Context.h
        src/include/play/game/GameLoop.h
        src/include/play/game/FNFObject.h
        src/include/data/animation/TextureAtlas.h
        src/src/data/stage/StageData.cpp
        src/include/data/stage/StageRegistry.h
        src/include/data/notestyle/NoteStyleRegistry.h
)

set(CMAKE_AUTORCC ON)

if(QT_VERSION VERSION_LESS 5.15)
    qt5_add_resources(PROJECT_SOURCES qml.qrc)
elseif(QT_VERSION VERSION_LESS 6.2)
    qt_add_resources(PROJECT_SOURCES qml.qrc)
endif()



# ===================== 避免野指针的利器ASan ================================
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -O0")
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
# 用的时候去掉注释！
# =============== 对所有的都生效，记得Release版本删掉他们！ =====================

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# 预编译头加快编译
target_precompile_headers(${PROJECT_NAME} PRIVATE
        <QString>
        <QDir>
        <QFile>
        <QFileInfo>
        <QDebug>
        <QCoreApplication>
        <QStandardPaths>
        <QObject>
        <QVector>
        <QMap>
        <QHash>
        <QSet>
        <QStringList>
        <QVariant>
        <QJsonDocument>
        <QJsonObject>
        <QJsonArray>

        <QQmlApplicationEngine>
        <QQmlContext>
        <QQuickView>
        <QApplication>
        <QIcon>

        <string>
        <vector>
        <map>
        <memory>
        <optional>
        <algorithm>
        <fstream>
        <sstream>
        <filesystem>

        <nlohmann/json.hpp>
        <miniaudio/miniaudio.h>
)

# 不再使用qrc系统，而是直接携带assets文件进行读取
if (NOT ANDROID)
    message(STATUS "资产源目录: ${CMAKE_CURRENT_SOURCE_DIR}/assets")
    message(STATUS "目标路径: $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
            COMMENT "（只有不是android才生效）正在把assets目录拷贝到构建目录..."
    )
endif ()

# 为了我们的权限和assets
if (ANDROID)
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "${CMAKE_CURRENT_SOURCE_DIR}/android/assets"
            COMMENT "正在把项目下的assets目录同步到android/assets用于apk打包...")

    set_target_properties(${PROJECT_NAME} PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)
endif ()

# CMake里面一定要加上qml文件，否则无法生效！
if(QT_VERSION VERSION_GREATER_EQUAL 6.2)
    qt_add_qml_module(${PROJECT_NAME}
        URI "fnf"
        VERSION 1.0
        RESOURCE_PREFIX "/qt/qml/"
        QML_FILES
            QML/main.qml
            QML/fonts/Mainfont.qml
            QML/views/modding/Modlist.qml
            QML/views/modding/ModItem.qml
            QML/views/modding/detailview/BottomBar.qml
            QML/views/modding/detailview/ModDetailView.qml
            QML/views/modding/detailview/sectionLoader/AssetPreview.qml
            QML/buttons/DefaultButton.qml
            QML/views/modding/EmptyModView.qml
            QML/views/info/ViceCityInfo.qml
    )
endif()

set_target_properties(${PROJECT_NAME}
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

# Windows特产，ico和rc文件主要是服务于这个
if (WIN32)
    set(app_icon_resource "${CMAKE_CURRENT_SOURCE_DIR}/BF.ico")
    set_source_files_properties("${app_icon_resource}" PROPERTIES HEADER_FILE_ONLY FALSE)
    target_sources(${PROJECT_NAME} PRIVATE "${app_icon_resource}")
endif ()


target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt::Quick
        Qt6::Widgets
)


target_include_directories(${PROJECT_NAME} PUBLIC src/include)
target_include_directories(${PROJECT_NAME} PUBLIC src/thirdParty)

if(Qt6_FOUND)
    qt_import_qml_plugins(${PROJECT_NAME})
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
