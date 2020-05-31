### Build ###

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/30b8d23c44744ea8972c7a04c34b32ca)](https://app.codacy.com/manual/cleborys/nettools?utm_source=github.com&utm_medium=referral&utm_content=cleborys/nettools&utm_campaign=Badge_Grade_Dashboard)

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
