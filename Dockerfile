ARG COMPILER

FROM registry.gitlab.com/offa/docker-images/${COMPILER}:stable

RUN apt-get install -y --no-install-recommends pkg-config qt5-default libusb-1.0-0-dev lcov && \
        mkdir deps && cd deps && \
        if [ "$(echo ${CXX} | cut -c -5)" = "clang" ]; then \
            export CXXFLAGS="-stdlib=libc++" && \
            echo "Installation of cxxabi.h and __cxxabi_config.h" && \
            CXX_ABIURL=https://raw.githubusercontent.com/llvm-mirror/libcxxabi/master/include/ && \
            curl -sSL ${CXX_ABIURL}/cxxabi.h -o /usr/include/c++/v1/cxxabi.h && \
            curl -sSL ${CXX_ABIURL}/__cxxabi_config.h -o /usr/include/c++/v1/__cxxabi_config.h; \
        fi && \
        git clone  https://github.com/google/googletest.git && \
        cd googletest && \
        git checkout cafa9e52285d743d1394900fa95662950f712a72 && \
        mkdir build && cd build && \
        cmake .. && \
        make && make install && \
        cd ../.. && rm -rf deps

