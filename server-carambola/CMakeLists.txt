###################################################
#### SETTING UP CMAKE FOR CROSS-COMPILATION... ####
###################################################

# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Carambola)

# which compilers to use for C and C++
SET(OPENWRT_TOOLCHAIN_PATH /opt/carambola/toolchain-mipsel_r2_gcc-4.7-linaro_uClibc-0.9.33.2)
SET(CMAKE_C_COMPILER   ${OPENWRT_TOOLCHAIN_PATH}/bin/mipsel-openwrt-linux-gcc)
SET(CMAKE_CXX_COMPILER ${OPENWRT_TOOLCHAIN_PATH}/bin/mipsel-openwrt-linux-g++)

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH /opt/carambola/toolchain-mipsel_r2_gcc-4.7-linaro_uClibc-0.9.33.2)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_CXX_FLAGS "-Wall -O2")


###################################################
#### SETTING UP THE PROJECT                    ####
###################################################
SET(ROOT_DIR ..)
SET(server_sources
    ${ROOT_DIR}/server.cpp         
    ${ROOT_DIR}/Logger.cpp         
    ${ROOT_DIR}/Utils.cpp         
    ${ROOT_DIR}/SerialDevice.cpp
    ${ROOT_DIR}/DualMotorController.cpp
    ${ROOT_DIR}/CommandManager.cpp
    ${ROOT_DIR}/SocketManager.cpp
)

#INCLUDE_DIRECTORIES(${ROOT_DIR})

ADD_EXECUTABLE(server.carambola ${server_sources} )

TARGET_LINK_LIBRARIES(server.carambola pthread)

