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
add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC include)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES SUFFIX ".asi")
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
)

if(DEFINED ENV{GTA_SA_DIR})
    set(INSTALL_DIR $ENV{GTA_SA_DIR}/scripts)
else()
    message(FATAL_ERROR "GTA_SA_DIR not defined in your enviroment")
endif()

install(TARGETS ${CMAKE_PROJECT_NAME}
    RUNTIME DESTINATION ${INSTALL_DIR}
)
