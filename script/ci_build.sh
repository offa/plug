#!/bin/bash

set -ex


BUILD_ARGS=("-DCMAKE_BUILD_TYPE=Debug")
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
            ;;
    esac
done



if [[ "${CXX}" == clang* ]]
then
    export CXXFLAGS="-stdlib=libc++"
fi

mkdir -p build && cd build
cmake "${BUILD_ARGS[@]}" ..
make
make unittest


if [[ "${COVERAGE}" == true ]]
then
    make coverage
fi

