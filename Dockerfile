ARG COMPILER

FROM registry.gitlab.com/offa/docker-images/${COMPILER}:stable

RUN apt-get install -y --no-install-recommends pkg-config \
                                            qt5-default \
                                            libusb-1.0-0-dev && \
                                            mkdir deps && cd deps && \
                                            git clone --depth=1 https://github.com/google/googletest.git && \
                                            cd googletest && mkdir build && cd build && \
                                            cmake .. && \
                                            make && make install && \
                                            cd ../.. && rm -rf deps


