cmake_minimum_required(VERSION 3.16)
project(danmaku
  VERSION 1.0.1.0
  DESCRIPTION "Danmaku Client"
  HOMEPAGE_URL "https://github.com/hly1204/danmaku4bilibili"
  LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets WebSockets)

add_executable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME}
  PROPERTIES
    POSITION_INDEPENDENT_CODE True
    WIN32_EXECUTABLE $<IF:$<CONFIG:Debug>,False,True>
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
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::WebSockets)

if (MSVC)

  if (Qt6_DIR)
    set(WIN_DEPLOY_QT ${Qt6_DIR}/../../../bin/windeployqt)
    set(LRELEASE ${Qt6_DIR}/../../../bin/lrelease)
    set(LUPDATE ${Qt6_DIR}/../../../bin/lupdate)
  elseif (CMAKE_PREFIX_PATH)
    set(WIN_DEPLOY_QT ${CMAKE_PREFIX_PATH}/bin/windeployqt)
    set(LRELEASE ${CMAKE_PREFIX_PATH}/bin/lrelease)
    set(LUPDATE ${CMAKE_PREFIX_PATH}/bin/lupdate)
  endif ()

  if (WIN_DEPLOY_QT AND LRELEASE)
    add_custom_command(TARGET ${PROJECT_NAME}
      POST_BUILD
      COMMAND SET
      ARGS VCINSTALLDIR=${CMAKE_CXX_COMPILER}/../../../../../../../
      COMMAND ${WIN_DEPLOY_QT}
      ARGS $<$<CONFIG:Debug>:--debug> $<$<CONFIG:Release>:--release> $<TARGET_FILE:${PROJECT_NAME}>
      COMMAND ${CMAKE_COMMAND} -E
      ARGS make_directory $<TARGET_FILE_DIR:${PROJECT_NAME}>/i18n
      COMMAND ${LRELEASE}
      ARGS ${CMAKE_SOURCE_DIR}/i18n/danmaku_zh_CN.ts -qm $<TARGET_FILE_DIR:${PROJECT_NAME}>/i18n/danmaku_zh_CN.qm)
  endif ()

  if (LUPDATE)
    add_custom_command(TARGET ${PROJECT_NAME}
      PRE_BUILD
      COMMAND ${LUPDATE}
      ARGS ${SOURCE_FILES} -ts ${CMAKE_SOURCE_DIR}/i18n/danmaku_zh_CN.ts)
  endif ()

endif ()
