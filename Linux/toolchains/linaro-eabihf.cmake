
include(CMakeForceCompiler)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION)

set(CMAKE_C_COMPILER /opt/toolchains/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER /opt/toolchains/gcc-linaro-6.2.1-2016.11-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++)

#set(CMAKE_FIND_ROOT_PATH )

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)