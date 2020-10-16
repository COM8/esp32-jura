cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

set(SOURCES Esp32Jura.cpp

            utils/SerialJuraBridgeTask.cpp
            
            jura/JuraConnection.cpp
            jura/CoffeeMaker.cpp
            jura/CoffeeMakerTask.cpp
            jura/JuraSnooperTask.cpp

            esp/Storage.cpp
            esp/WifiTask.cpp

            esp/bt/BluetoothServer.cpp
            esp/bt/BLECharacteristicRWCallback.cpp
            esp/bt/BLEServiceHelper.cpp
            esp/bt/BluetoothServerCallback.cpp

            esp/sensor/Button.cpp

            esp/actuator/RgbLed.cpp
            
            esp/serial/SerialConnection.cpp
            esp/serial/SerialConnectionTask.cpp
            
            xmpp/XmppClient.cpp
            xmpp/XmppConnection.cpp
            xmpp/XmppAccount.cpp
            xmpp/Jid.cpp
            xmpp/XmppTask.cpp
            xmpp/XmppUtils.cpp

            xmpp/tcp/TcpConnection.cpp
            xmpp/tcp/XmppProtocol.cpp
            xmpp/tcp/XmppPacket.cpp
            
            xmpp/messages/MessageParser.cpp
            xmpp/messages/Message.cpp

            xmpp/messages/AbstractMessage.cpp
            xmpp/messages/AbstractAddressableMessage.cpp
            xmpp/messages/IQMessage.cpp
            xmpp/messages/BindResourceMessage.cpp

            xmpp/messages/xep_iot/Device.cpp
            xmpp/messages/xep_iot/AbstractNode.cpp
            xmpp/messages/xep_iot/SensorNode.cpp
            xmpp/messages/xep_iot/ActuatorNode.cpp
            xmpp/messages/xep_iot/ButtonNode.cpp
            xmpp/messages/xep_iot/HeaderNode.cpp
            xmpp/messages/xep_iot/TextSingleNode.cpp
            
            xmpp/crypto/CryptoUtils.cpp
            
            xmpp/helpers/PubSubHelper.cpp)

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR} $ENV{IDF_PATH}/components
                       REQUIRES smooth_component bt cpp_utils mbedtls esp_adc_cal)

# -----------------------Tiny-XML-2-----------------------
# Build static library, do not build test executables
option(BUILD_SHARED_LIBS OFF)
option(BUILD_TESTING OFF)

# Unfortunately the library performs install and export. Would 
# have been nice if devs made that an option like BUILD_SHARED_LIBS
# and BUILD_TESTING. Override install() and export() to do nothing
# instead.
function(install)
endfunction()

function(export)
endfunction()

# Import tinyxml2 targets 
add_subdirectory(xmpp/tinyxml2)

# Link tinyxml2 to main component
target_link_libraries(${COMPONENT_LIB} PUBLIC tinyxml2)
