### Build ###
```bash
mkdir build
cd build
cmake ..
make
```

### Run tests ###
Having built as above, run
```bash
ctest -V
```
To get coverage metrics, compile as follows:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make coverage
```
and inspect `build/coverage/index.html`
