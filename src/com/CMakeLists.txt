
add_library(plug-mustang Mustang.cpp PacketSerializer.cpp Packet.cpp)
add_library(plug-communication
    UsbComm.cpp
    ConnectionFactory.cpp
    )

add_library(plug-communication-usb
    UsbContext.cpp
    UsbException.cpp
    UsbDevice.cpp
    )

add_library(plug-libusb LibUsbCompat.cpp)
target_link_libraries(plug-libusb PUBLIC libusb-1.0::libusb-1.0)

add_library(plug-updater MustangUpdater.cpp)
