cmake_minimum_required(VERSION 3.16)
project(danmaku
  VERSION 1.0.1.0
  DESCRIPTION "Danmaku Client"
  HOMEPAGE_URL "https://github.com/hly1204/danmaku4bilibili"
  LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network WebSockets)

qt_add_executable(${PROJECT_NAME})
qt_add_resources(${PROJECT_NAME} "translations"
  PREFIX "/"
  BASE ${Qt6_DIR}/../../../
  FILES ${Qt6_DIR}/../../../translations/qtbase_zh_CN.qm)

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    POSITION_INDEPENDENT_CODE True
    WIN32_EXECUTABLE $<IF:$<CONFIG:Debug>,False,True>
    MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug> # 动态库版本的 Qt 需要加 DLL 后缀
    # MSVC_TOOLSET_VERSION 143
    COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG_OUTPUT
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
    OUTPUT_NAME ${PROJECT_NAME}
    DEBUG_POSTFIX d
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION})
if (MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
  configure_file(win32/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)
  file(GLOB SOURCE_FILES src/* ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
else ()
  file(GLOB SOURCE_FILES src/*)
endif ()
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Network Qt6::WebSockets)
