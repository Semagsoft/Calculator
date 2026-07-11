cmake_minimum_required(VERSION 3.16)

project(Calculator VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(Calculator
    src/main.cpp
    src/CalculatorEngine.cpp
    src/CalculatorWidget.cpp
    src/ExpressionParser.cpp
    include/CalculatorWidget.h
    include/CalculatorEngine.h
    include/ExpressionParser.h
)

target_include_directories(Calculator PRIVATE include)
target_link_libraries(Calculator PRIVATE Qt6::Widgets)

install(TARGETS Calculator RUNTIME DESTINATION bin)

install(FILES package/calculator-1.0-linux-x86_64/share/applications/calculator.desktop
    DESTINATION share/applications)

install(FILES package/calculator-1.0-linux-x86_64/share/icons/hicolor/256x256/apps/calculator.png
    DESTINATION share/icons/hicolor/256x256/apps)

install(FILES package/calculator-1.0-linux-x86_64/share/icons/hicolor/scalable/apps/calculator.svg
    DESTINATION share/icons/hicolor/scalable/apps)

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
