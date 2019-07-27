# AGS laboratory work, 6 semester
## Requirements
* GNU/Linux distribution
* Compiler that supports C++17
* CMake
* libGL
* FreeGLUT
* GLEW
* GLM
* DevIL

## Build
```
$ git clone --recursive https://github.com/Twi1ightSpark1e/AGSlab.git
$ cd AGSlab
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
$ make
```

#### With Clang instead of GCC
All the same as in previous step, but slightly different `cmake` command:

`$ env CC=$(which clang) CXX=$(which clang++) CXXFLAGS="-stdlib=libc++" cmake .. -DCMAKE_BUILD_TYPE=Release`

