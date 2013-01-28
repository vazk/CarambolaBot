###################################################
#### SETTING UP THE PROJECT                    ####
###################################################
SET(ROOT_DIR ..)
SET(server_sources
    ${ROOT_DIR}/client.cpp         
    ${ROOT_DIR}/Logger.cpp         
    ${ROOT_DIR}/Utils.cpp         
    ${ROOT_DIR}/SerialDevice.cpp
    ${ROOT_DIR}/SocketDevice.cpp
)

ADD_EXECUTABLE(client.x86 ${server_sources})

