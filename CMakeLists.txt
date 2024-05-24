cmake_minimum_required(VERSION 3.26.3)

project(ErrorHandlerSA VERSION 1.0
                       LANGUAGES CXX)

# Enable the export of compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Project setup standards
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)

# Add libs to project
add_subdirectory(libs/spdlog EXCLUDE_FROM_ALL SYSTEM)

# Project target setup
file(GLOB SOURCES
    "src/*.cpp"
    "src/*.h"
)
add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE spdlog)

set_target_properties(${CMAKE_PROJECT_NAME} 
    PROPERTIES
    SUFFIX ".asi"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    _CRT_SECURE_NO_WARNINGS
)

# Install rotine
if(DEFINED ENV{GTA_SA_DIR})
    set(INSTALL_DIR $ENV{GTA_SA_DIR}/scripts)
else()
    message(FATAL_ERROR "GTA_SA_DIR not defined in your enviroment")
endif()

install(TARGETS ${CMAKE_PROJECT_NAME}
    RUNTIME DESTINATION ${INSTALL_DIR}
)
