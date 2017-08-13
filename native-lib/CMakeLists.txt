cmake_minimum_required(VERSION 3.7)
project(toolkit)

message("-- CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message("-- JAVA_HOME: ${JAVA_HOME}")

find_package(JNI REQUIRED)
include_directories(${JNI_INCLUDE_DIRS})

set(SRC src/main.c)

add_library(${PROJECT_NAME}-${PLATFORM_NAME} SHARED ${SRC})
target_link_libraries(${PROJECT_NAME}-${PLATFORM_NAME} ${JNI_LIBRARIES})
