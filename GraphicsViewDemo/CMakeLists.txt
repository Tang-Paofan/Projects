cmake_minimum_required(VERSION 3.5.0)
project(GraphicsView VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH E:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5/)
# set(Qt5_DIR E:/Qt/5.15.2/msvc2019_64/lib/cmake/Qt5/)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

# 添加可执行文件，注意添加WIN32，否则有一个控制台黑框
add_executable(GraphicsView
src/main.cpp
src/widget.h src/widget.cpp
src/BezierCurveItem.h src/BezierCurveItem.cpp
src/ScatterItem.h src/ScatterItem.cpp
)

# 链接Qt库
target_link_libraries(GraphicsView Qt5::Widgets)