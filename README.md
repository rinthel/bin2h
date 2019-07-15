# bin2h

simple binary-to-c(cpp)header converter.

### Feature

- merge multiple binary files into one header source code
- set variable names of each binary file automatically by UPPER_SNAKE_CASE

### Build

bin2h use the cmake build setting. Please make sure that cmake is installed in your machine.

- use with a single configuration project generator (i.e. unix makefiles)
```sh
> cmake -H. -Bcmake-build-release -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./install
> cmake --build cmake-build-release --target install
```

- use with a multiple configuration project generator (i.e. xcode, VC++)
```sh
> cmake -H. -Bbuild -DCMAKE_INSTALL_PREFIX=./install
> cmake --build build --config Release --target install
```

### Usage

```sh
> bin2h <inputFilename>... -o <outputFilename> [-v] [-c] [-a align-byte-size]
```

- `-v`: verbose mode
- `-c`: visualize binary data as a comment
- `-a`: resulting byte array would be byte-aligned with specific size
