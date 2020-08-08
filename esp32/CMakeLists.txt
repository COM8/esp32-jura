cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT UNIX)
    message(FATAL_ERROR "Unsupported OS")
endif ()

if(${ESP_PLATFORM})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)

    # Include Smooth as a component
    set(EXTRA_COMPONENT_DIRS externals/smooth/smooth_component)

    project(esp32Jura)
else()
    # Empty project when not building for ESP (i.e. when loading the project into an IDE with already configured tool chains for native Linux)
endif()