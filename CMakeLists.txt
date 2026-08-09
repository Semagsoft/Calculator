cmake_minimum_required(VERSION 3.16)

project(Calculator VERSION 1.01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Calculator WIN32
    src/main.cpp
    src/CalculatorEngine.cpp
    src/CalculatorWidget.cpp
    src/ExpressionParser.cpp
    include/CalculatorWidget.h
    include/CalculatorEngine.h
    include/ExpressionParser.h
)

set(CALC_ICON_SVG "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/calculator.svg")
set(CALC_ICON_PNG "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/calculator.png")
set(CALC_ICON_ICO "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/calculator.ico")

find_program(RSVG_CONVERT_EXECUTABLE rsvg-convert
    HINTS "C:/msys64/ucrt64/bin" "C:/msys64/mingw64/bin" "C:/msys64/mingw32/bin")

if(RSVG_CONVERT_EXECUTABLE)
    add_custom_command(
        OUTPUT "${CALC_ICON_PNG}"
        COMMAND "${RSVG_CONVERT_EXECUTABLE}" -w 256 -h 256
                -o "${CALC_ICON_PNG}" "${CALC_ICON_SVG}"
        DEPENDS "${CALC_ICON_SVG}"
        COMMENT "Generating calculator.png from calculator.svg"
        VERBATIM
    )
elseif(NOT EXISTS "${CALC_ICON_PNG}")
    message(FATAL_ERROR
        "rsvg-convert not found and ${CALC_ICON_PNG} is missing. "
        "Install rsvg-convert (librsvg2-bin / librsvg / mingw-w64-ucrt-x86_64-librsvg) "
        "or restore the icon file.")
endif()

if(WIN32)
    if(RSVG_CONVERT_EXECUTABLE)
        add_custom_command(
            OUTPUT "${CALC_ICON_ICO}"
            COMMAND powershell -NoProfile -ExecutionPolicy Bypass
                    -File "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate-ico.ps1"
                    -Svg "${CALC_ICON_SVG}" -Out "${CALC_ICON_ICO}"
            DEPENDS "${CALC_ICON_SVG}"
            COMMENT "Generating calculator.ico from calculator.svg"
            VERBATIM
        )
    elseif(NOT EXISTS "${CALC_ICON_ICO}")
        message(FATAL_ERROR
            "rsvg-convert not found and ${CALC_ICON_ICO} is missing. "
            "Install rsvg-convert (mingw-w64-ucrt-x86_64-librsvg) or restore the icon file.")
    endif()

    target_sources(Calculator PRIVATE resources/calculator.rc)
    set_property(SOURCE resources/calculator.rc APPEND PROPERTY
        OBJECT_DEPENDS "${CALC_ICON_ICO}")
endif()

set(CALC_ICON_OUTPUTS "${CALC_ICON_PNG}")
if(WIN32)
    list(APPEND CALC_ICON_OUTPUTS "${CALC_ICON_ICO}")
endif()
add_custom_target(CalculatorIcons ALL DEPENDS ${CALC_ICON_OUTPUTS})

target_include_directories(Calculator PRIVATE include)
target_link_libraries(Calculator PRIVATE Qt6::Widgets)

if(APPLE)
    set_target_properties(Calculator PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "Calculator"
        MACOSX_BUNDLE_GUI_IDENTIFIER "io.github.primo.Calculator"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_ICON_FILE "calculator"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/package/Info.plist.in"
    )
endif()

install(FILES resources/applications/calculator.desktop
    DESTINATION share/applications)

install(FILES resources/icons/calculator.png
    DESTINATION share/icons/hicolor/256x256/apps)

install(FILES resources/icons/calculator.svg
    DESTINATION share/icons/hicolor/scalable/apps)

install(FILES resources/metainfo/calculator.metainfo.xml
    DESTINATION share/metainfo)

if(APPLE)
    install(TARGETS Calculator BUNDLE DESTINATION .)
else()
    install(TARGETS Calculator RUNTIME DESTINATION bin)
endif()

option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_executable(CalculatorTests
        tests/test_main.cpp
        src/CalculatorEngine.cpp
        src/ExpressionParser.cpp
        include/CalculatorEngine.h
        include/ExpressionParser.h
    )
    target_include_directories(CalculatorTests PRIVATE include)
    add_test(NAME CalculatorTests COMMAND CalculatorTests)

    find_package(Qt6 QUIET COMPONENTS Test)
    if(Qt6Test_FOUND)
        add_executable(CalculatorUITests
            tests/ui_test_main.cpp
            src/CalculatorWidget.cpp
            src/CalculatorEngine.cpp
            src/ExpressionParser.cpp
            include/CalculatorWidget.h
            include/CalculatorEngine.h
            include/ExpressionParser.h
        )
        target_include_directories(CalculatorUITests PRIVATE include)
        target_link_libraries(CalculatorUITests PRIVATE Qt6::Test Qt6::Widgets)
        add_test(NAME CalculatorUITests COMMAND CalculatorUITests)
    endif()
endif()
