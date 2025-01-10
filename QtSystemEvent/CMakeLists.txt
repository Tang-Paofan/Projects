cmake_minimum_required(VERSION 3.5.0)
project(QtSystemEvent VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Widgets Concurrent REQUIRED)

add_executable(QtSystemEvent
main.cpp
SystemEventType.h
SystemEventHandler.h
SystemEventManager.h SystemEventManager.cpp)

# 链接Qt库
target_link_libraries(QtSystemEvent PRIVATE Qt5::Widgets Qt5::Concurrent)

set_target_properties(QtSystemEvent PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)