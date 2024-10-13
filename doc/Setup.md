# Setup

List of packages required to build the software.

For development [*Google Test*](https://github.com/google/googletest) (`gtest`) is needed for testing.

It can be installed via the system package manger, [Conan](https://github.com/conan-io/conan) or build from source.

## Arch Linux

```sh
sudo pacman -S \
    cmake \
    libusb \
    qt6-base
```

## Debian / Ubuntu

```sh
sudo apt-get install \
    pkg-config \
    qt6-base-dev \
    libglx-dev \
    libgl1-mesa-dev \
    libusb-1.0-0-dev
```

