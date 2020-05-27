
add_library(plug-mustang Mustang.cpp PacketSerializer.cpp Packet.cpp)
add_library(plug-communication UsbComm.cpp ConnectionFactory.cpp)
add_library(plug-updater MustangUpdater.cpp)
