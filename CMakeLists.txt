cmake_minimum_required(VERSION 3.16)
project("Friday_Night_Funkin" LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Quick
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    resources.rc
        default_resources.qrc
        src/include/audio/FunkinSound.h
        src/include/audio/FunkinSoundSystem.h
        src/include/audio/SoundEnum.h
        src/include/save/Save.h
        src/include/save/SaveSystem.h
        src/include/ui/buttons/FButton.h
        src/include/ui/buttons/FButton.qml
        src/include/utils/GlobalSystemUtils.h
        src/include/utils/InstanceLibrary.h
        src/include/utils/Math.h
        src/include/utils/Path.h
        src/src/audio/FunkinSoundSystem.cpp
        src/src/audio/MiniaudioOggImpl.cpp
        src/src/utils/Path.cpp
        src/include/gameloop/GameLoop.h
)

set(CMAKE_AUTORCC ON)

if(QT_VERSION VERSION_LESS 5.15)
    qt5_add_resources(PROJECT_SOURCES qml.qrc)
elseif(QT_VERSION VERSION_LESS 6.2)
    qt_add_resources(PROJECT_SOURCES qml.qrc)
endif()

qt_add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# CMake里面一定要加上qml文件，否则无法生效！
if(QT_VERSION VERSION_GREATER_EQUAL 6.2)
    qt_add_qml_module(${PROJECT_NAME}
        URI "friday_night_funkin"
        VERSION 1.0
        RESOURCE_PREFIX "/qt/qml/"
        QML_FILES
            QML/main.qml
            QML/fonts/MainFont.qml
        RESOURCES
            BF.ico
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
)

target_include_directories(${PROJECT_NAME} PUBLIC src/include)
target_include_directories(${PROJECT_NAME} PUBLIC src/thirdParty)

if(Qt6_FOUND)
    qt_import_qml_plugins(${PROJECT_NAME})
endif()
