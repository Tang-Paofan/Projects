cmake_minimum_required(VERSION 3.5.0)

project(CrashReporter VERSION 0.1.0.0)

# 指定为C++17 版本
set(CMAKE_CXX_STANDARD 17)

# 添加预编译宏
add_definitions(-DCRASHREPORTER_LIB)

configure_file(
  ${CMAKE_SOURCE_DIR}/version.rc.in
  ${CMAKE_BINARY_DIR}/version.rc
  @ONLY)

file(GLOB SRC_LIST "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.h")
file(GLOB RC_FILE ${CMAKE_BINARY_DIR}/*.rc)

add_library(${PROJECT_NAME} STATIC ${SRC_LIST} ${RC_FILE})

# # 设置可执行文件的输出目录
# set_target_properties(MyExecutable PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
# )

# 设置共享库的输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)

# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE "Release")
# endif()

# include(CMakePrintHelpers)
# cmake_print_variables(CMAKE_BUILD_TYPE CMAKE_C_COMPILER CMAKE_CXX_COMPILER)
# message(STATUS ${CMAKE_CONFIGURATION_TYPES})
# message(STATUS ${CMAKE_BUILD_TYPE})
# message(STATUS ${build_command()})
# message(STATUS ${EXECUTABLE_OUTPUT_PATH})

# # 检查构建类型并设置自定义变量
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(MY_CUSTOM_VAR "value1")
# elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#     set(MY_CUSTOM_VAR "value2")
# endif()

# 自定义事件，构建后拷贝文件
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME}.h ${CMAKE_SOURCE_DIR}/include
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/src/defines.h ${CMAKE_SOURCE_DIR}/include
  # COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.lib ${CMAKE_SOURCE_DIR}/lib
  # COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.dll ${CMAKE_SOURCE_DIR}/bin
  # COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${PROJECT_NAME}.pdb ${CMAKE_SOURCE_DIR}/bin
  # COMMAND ${CMAKE_COMMAND} -E echo "Files copied successfully!" || echo "Ignoring error..."
)
