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
            pip3 install -U gcovr

            GCC_VERSION="$(${CC} -dumpfullversion | cut -f1 -d.)"
            ln -sf /usr/bin/gcov-${GCC_VERSION} /usr/bin/gcov
            ;;
    esac
done

BUILD_ARGS+=("-DCMAKE_BUILD_TYPE=${BUILD_TYPE}")
export GTEST_BRIEF=1


if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"

    # Workaround for #228
    export ASAN_OPTIONS=alloc_dealloc_mismatch=0
fi


mkdir -p build && cd build
cmake "${BUILD_ARGS[@]}" ..
make
make unittest


if [[ "${COVERAGE}" == true ]]
then
    make coverage
fi
