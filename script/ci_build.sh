#!/bin/bash

set -ex

declare -a BUILD_ARGS
BUILD_TYPE="Release"
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
fi

export GTEST_BRIEF=1

mkdir -p build && cd build
cmake "${BUILD_ARGS[@]}" ..
make
make unittest


if [[ "${COVERAGE}" == true ]]
then
    make coverage
fi
