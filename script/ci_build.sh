#!/bin/bash

set -ex

declare -a BUILD_ARGS
BUILD_TYPE="Debug"
COVERAGE=false

for arg in "$@"
do
    case "${arg}" in
        -asan)
            BUILD_ARGS+=("-DSANITIZER_ASAN=ON")
            ;;
        -ubsan)
            BUILD_ARGS+=("-DSANITIZER_UBSAN=ON")
            ;;
        -cov)
            BUILD_ARGS+=("-DCOVERAGE=ON")
            COVERAGE=true;
            BUILD_TYPE="Debug"
            apt-get install -y --no-install-recommends python3-pip python3-pkg-resources python3-setuptools
            pip3 install gcovr
            ;;
    esac
done

BUILD_ARGS+=("-DCMAKE_BUILD_TYPE=${BUILD_TYPE}")


if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"

    # Workaround for #228
    export ASAN_OPTIONS=alloc_dealloc_mismatch=0
fi


# Install dependencies
apt-get update
apt-get install -y --no-install-recommends \
        pkg-config \
        qtbase5-dev \
        libusb-1.0-0-dev

git clone --depth=1 --branch=release-1.12.1 https://github.com/google/googletest.git
mkdir googletest/build
pushd googletest/build
cmake -DCMAKE_CXX_STANDARD=17 ..
make -j install
export GTEST_BRIEF=1
popd


mkdir -p build && cd build
cmake "${BUILD_ARGS[@]}" ..
make
make unittest


if [[ "${COVERAGE}" == true ]]
then
    make coverage
fi
