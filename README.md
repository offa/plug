# Plug

### [[GitHub](https://github.com/offa/plug)] [[GitLab](https://gitlab.com/offa/plug)]

[![CI](https://github.com/offa/plug/workflows/ci/badge.svg)](https://github.com/offa/plug/actions)
[![Pipeline Status](https://gitlab.com/offa/plug/badges/master/pipeline.svg)](https://gitlab.com/offa/plug/commits/master)
[![Coverage Report](https://gitlab.com/offa/plug/badges/master/coverage.svg)](https://gitlab.com/offa/plug/commits/master)
[![GitHub release](https://img.shields.io/github/release/offa/plug.svg)](https://github.com/offa/plug/releases)
[![License](https://img.shields.io/badge/license-GPLv3-yellow.svg)](LICENSE)
![C++](https://img.shields.io/badge/c++-20-green.svg)

Software for Fender Mustang Amps. This is a fork of ***piorekf's Plug***.

Please see [Contributing](CONTRIBUTING.md) for how to contribute to this project.


## Requirements

- [**Qt6**](https://www.qt.io/)
- [**libusb-1.0**](http://libusb.info/)



## Building

Building and testing is done through CMake:

```
mkdir build && cd build
cmake ..
make
make unittest
```


## Installation

CMake will install the application and *udev* rules using:

```
make install
```

The *udev* rule allows the USB access without *root* for the users of the `plugdev` group.


## Libusb Logging

Debug message logging of [*libusb*](https://libusb.sourceforge.io/api-1.0/) can be controlled by the `LIBUSB_DEBUG` variable (0: None, 1: Error, 2: Warning, 3: Info, 4: Debug).


## Credits

Thanks to *piorekf* and all Plug contributors.


## License

**GNU General Public License (GPL)**

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
