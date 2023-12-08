#!/bin/bash

set -ex

if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
fi

# Install dependencies
apt-get update
apt-get install -y --no-install-recommends \
        pkg-config \
        qt6-base-dev \
        libglx-dev \
        libgl1-mesa-dev \
        libusb-1.0-0-dev

git clone --depth=1 --branch=release-1.12.1 https://github.com/google/googletest.git
mkdir googletest/build
cd googletest/build
cmake -DCMAKE_CXX_STANDARD=17 ..
make -j install
