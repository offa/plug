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

git clone --depth=1 --branch=v1.15.0 https://github.com/google/googletest.git
mkdir googletest/build
cd googletest/build
cmake -DCMAKE_CXX_STANDARD=20 ..
make -j install
