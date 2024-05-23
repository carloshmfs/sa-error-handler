cmake_minimum_required(VERSION 3.26.3)

project(ErrorHandlerSA VERSION 1.0
                       LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (MSVC)
    set(CMAKE_GENERATOR_PLATFORM Win32)
endif()

file(GLOB SOURCES
    "src/*.cpp"
    "src/*.h"
)
message(${SOURCES})
add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC include)
 
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
)
