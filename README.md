# bin2h

simple binary-to-c(cpp)header converter.

### Feature

- merge multiple binary files into one header source code
- set variable names of each binary file automatically by UPPER_SNAKE_CASE
- can align an output byte array with the user-specified byte size
- can visualize an output byte array as ASCII codes
- can check input file's MD5 checksum and compare it with the checksum stored
  in the output file that already exists before generating a new output header file

### Dependency

- all dependencies are in the repository or managed by `Dependency.cmake`
- **[spdlog](https://github.com/gabime/spdlog.git)** for logging
- **[clipp](https://github.com/muellan/clipp.git)** for parsing command arguments
- **[catch2](https://github.com/catchorg/Catch2.git)** for unit testing
- **[picohash](https://github.com/catchorg/Catch2.git)** for computing a MD5 checksum

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
    [-l byte-count-per-line] [-e] [-f]
```

- `-v`: verbose mode
- `-c`: visualize binary data as a comment
- `-a`: resulting byte array would be byte-aligned with specific size (default: 1)
- `-l`: byte count per line (default: 8)
- `-e`: include input file's extension for naming array variables
- `-f`: force to generate output, although input does not change
