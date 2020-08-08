cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES Esp32Jura.cpp
            utils/SerialJuraBridgeTask.cpp
            jura/CoffeeMakerTask.cpp
            jura/JuraSnooperTask.cpp
            jura/JuraConnection.cpp
            esp/sensor/Button.cpp
            esp/serial/SerialConnection.cpp
            esp/serial/SerialConnectionTask.cpp)

idf_component_register(SRCS ${SOURCES}
        INCLUDE_DIRS
            ${CMAKE_CURRENT_LIST_DIR}
            $ENV{IDF_PATH}/components
        REQUIRES
            smooth_component
        )