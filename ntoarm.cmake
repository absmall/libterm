SET(CMAKE_SYSTEM_NAME qnx)
SET(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
SET(CMAKE_C_COMPILER   $ENV{QNX_HOST}/usr/bin/ntoarmv7-gcc)
SET(CMAKE_CXX_COMPILER $ENV{QNX_HOST}/usr/bin/ntoarmv7-g++)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  ${QNX_TARGET} ${QNX_TARGET}/armle-v7)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#Force -pfPIC when ompiling.
SET(CMAKE_C_FLAGS -fPIC)
