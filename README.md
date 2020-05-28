

### Build ###
```
mkdir build
cd build
cmake ..
make
```

### Run tests ###
Having built as abive, run
```
ctest -V
```
To get coverage metrics, compile as follows:
```
cmake -DCMAKE_BUILD_TYPE=Debug ..
make coverage
```
and inspect `build/coverage/index.html`

