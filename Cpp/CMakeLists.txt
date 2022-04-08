cmake_minimum_required(VERSION 3.21)
project(Bezier)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "/usr/include/x86_64-linux-gnu/qt5")

find_package(Qt5 COMPONENTS
        Core
        Widgets
        REQUIRED)

add_executable(Bezier src/main.cpp src/MovablePoint.h src/MovablePoint.cpp src/MainWindow.h src/MainWindow.cpp src/Point.cpp src/Point.h)
target_link_libraries(Bezier
        Qt5::Core
        Qt5::Widgets
        )

