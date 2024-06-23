cmake_minimum_required(VERSION 3.21)

project(xmpp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Core Network)
find_package(PahoMqttCpp REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp")

add_executable(xmppclient ${PROJECT_SOURCES})

target_include_directories(xmppclient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6Network_INCLUDE_DIRS}
    ${PahoMqttCpp_INCLUDE_DIRS}
    /usr/local/include/QXmppQt6
)

target_link_libraries(xmppclient PRIVATE
    /pqc/ref/lib/libpqcrystals_kyber768_ref.so
    /pqc/ref/lib/libpqcrystals_fips202_ref.so
    ssl
    crypto
)

target_link_libraries(xmppclient PRIVATE
    Qt6::Core
    Qt6::Network
    /usr/local/lib/libQXmppQt6.so
    PahoMqttCpp::paho-mqttpp3
)

set_target_properties(xmppclient PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)
