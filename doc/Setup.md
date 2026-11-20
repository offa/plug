# Setup

Packages required to build the software, on top of CMake (≥ 3.12) and a C++20-capable compiler (e.g. GCC or Clang).

Unit tests are built by default. End users can disable them with the CMake option `PLUG_UNITTEST` (e.g. `cmake -DPLUG_UNITTEST=OFF ..`), which also avoids the development dependencies below.

## Arch Linux

```sh
sudo pacman -S \
    base-devel \
    cmake \
    libusb \
    qt6-base
```

## Debian / Ubuntu

```sh
sudo apt-get install \
    build-essential \
    cmake \
    pkg-config \
    qt6-base-dev \
    libglx-dev \
    libgl1-mesa-dev \
    libusb-1.0-0-dev
```

## Development dependencies

For development [*Google Test*](https://github.com/google/googletest) (`gtest`) is needed for testing.

It can be installed via the system package manager (Arch: `gtest`, Debian/Ubuntu: `libgtest-dev libgmock-dev`), [Conan](https://conan.io/center/recipes/gtest) or built from source.
