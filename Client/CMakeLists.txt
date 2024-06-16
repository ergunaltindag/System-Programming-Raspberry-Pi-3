cmake_minimum_required(VERSION 3.0)
project(SistemProgramlama)

# İstemci Node.cpp dosyasını derle
add_executable(ClientNode ClientNode.cpp)

# SocketConLib.cpp ve SocketConLib.h dosyalarını derle
add_library(SocketConLib SocketConLib.cpp SocketConLib.h)

# İstemci Node'nun SocketConLib kütüphanesine bağımlılığını belirt
target_link_libraries(ClientNode PRIVATE SocketConLib)

