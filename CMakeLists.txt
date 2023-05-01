cmake_minimum_required (VERSION 3.11)

# Project declaration
set(MQTT_MAJOR_VERSION 3)
set(MQTT_MINOR_VERSION 1)
set(MQTT_MICRO_VERSION 2)
set(MQTT_VERSION ${MQTT_MAJOR_VERSION}.${MQTT_MINOR_VERSION}.${MQTT_MICRO_VERSION})
project(mqtt VERSION ${MQTT_VERSION})

add_executable(mqtt-sub network.cpp mqtt_subscribe.cpp)
target_link_libraries (mqtt-sub PRIVATE mosquitto)

add_executable(mqtt-pub network.cpp main_publish.cpp)
target_link_libraries (mqtt-pub PRIVATE mosquitto)