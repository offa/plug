# Plug

[![pipeline status](https://gitlab.com/offa/plug/badges/master/pipeline.svg)](https://gitlab.com/offa/plug/commits/master)
[![License](https://img.shields.io/badge/license-GPLv3-yellow.svg)](LICENSE)
[![C++](https://img.shields.io/badge/c++-17-green.svg)]()

Software for Fender Mustang Amps. This is a fork of [piorekf's Plug](https://bitbucket.org/piorekf/plug/).

Please see [Contributing](CONTRIBUTING.md) for how to contribute to this project.


## Goals

- Port to *Modern C++*
- Port to Qt5
- CMake support
- Modernization and Improvements
- Unit Test Suite
- Moving to Git


## Requirements

- [**CMake**](https://www.cmake.org/)
- [**Qt5**](https://www.qt.io/)
- [**libusb-1.0**](http://libusb.info/)



## Building

Building and Testing is done through CMake:

```
mkdir build && cd build
cmake ..
make
make unittest
```


## Installation

CMake will install the application and *udev* rule (`50-mustang.rules`) using:

```
make install
```

The *udev* rule will allow the USB access without *root* for the users of the `plugdev` group.


## Documentation

Visit the [Plug Website](https://bitbucket.org/piorekf/plug/) for documentation and technical details.


## Credits

Thanks to [piorekf and all Plug contributors](https://bitbucket.org/piorekf/plug/).


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



