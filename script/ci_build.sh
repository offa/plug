#!/bin/bash

set -ex

if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
fi

mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
make unittest
