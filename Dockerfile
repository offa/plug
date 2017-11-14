ARG COMPILER

FROM registry.gitlab.com/offa/docker-images/${COMPILER}:stable

RUN apt-get install -y --no-install-recommends pkg-config \
                                            qt5-default \
                                            libusb-1.0-0-dev

