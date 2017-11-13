FROM registry.gitlab.com/offa/docker-images/gcc-7:stable

RUN apt-get install -y --no-install-recommends pkg-config && \
                                            qt5-default && \
                                            libusb-1.0-0-dev 

