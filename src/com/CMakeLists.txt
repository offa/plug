
add_library(plug-mustang Mustang.cpp PacketSerializer.cpp Packet.cpp)
add_library(plug-communication
    UsbComm.cpp
    ConnectionFactory.cpp
    UsbContext.cpp
    UsbException.cpp
    UsbDevice.cpp
    )
add_library(plug-updater MustangUpdater.cpp)
