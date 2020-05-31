# nettools
C++ implementation of a few network tools built around C sockets for educative
purposes.

[![Build Status](https://travis-ci.com/cleborys/nettools.svg?branch=master)](https://travis-ci.com/cleborys/nettools)

## Build ##
Building requires `cmake` version 3.10 or higher.
```bash
mkdir build
cd build
cmake ..
make
```

## Run Tests ##
Having built as above, run
```bash
ctest -V
```
Note that creating "raw" ports (e.g. for ICMP) requires the appropriate rights,
so you might have to run `sudo make coverage` for these tests to pass.

To get code coverage metrics, compile as follows:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make coverage
```
and view `build/coverage/index.html`.
