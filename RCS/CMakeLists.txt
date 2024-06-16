cmake_minimum_required(VERSION 3.10)
project(RemoteControlSystem)

# Kütüphaneler
include_directories(include)



# Kaynak dosyalar
add_subdirectory(src)

# SocketConLib kütüphanesi
add_library(SocketConLib src/SocketConLib.cpp)
add_library(UartLib src/UartLib.cpp)
add_library(KeypadLib src/KeypadLib.cpp)
add_library(DigSensorLib src/DigSensorLib.cpp)
add_library(RelayLib src/RelayLib.cpp)
# İcra edilebilir dosyalar
add_executable(ServerNode ServerNode.cpp)
add_executable(EnvSensorNode EnvSensorNode.cpp)
add_executable(DigitalIONode DigitalIONode.cpp)

# Bağımlılıklar
target_link_libraries(ServerNode SocketConLib)
target_link_libraries(EnvSensorNode UartLib)
target_link_libraries(DigitalIONode KeypadLib DigSensorLib RelayLib)
target_link_libraries(DigitalIONode gpiod)