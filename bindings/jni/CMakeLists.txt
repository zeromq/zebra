cmake_minimum_required (VERSION 2.8)

project (zwebrapjni CXX)
enable_language (C)

find_package (JNI REQUIRED)
find_library (ZMQ_LIBRARY NAMES zmq libzmq REQUIRED)
find_library (ZWEBRAP_LIBRARY NAMES zwebrap REQUIRED)

include_directories (${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} src/native/include)

set (zwebrapjni_sources
   src/main/c/org_zeromq_zwebrap_ZebHandler.c
   src/main/c/org_zeromq_zwebrap_XrapMsg.c
   src/main/c/org_zeromq_zwebrap_ZwrClient.c
)

add_library (zwebrapjni SHARED ${zwebrapjni_sources})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -O2")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

target_link_libraries (zwebrapjni ${ZMQ_LIBRARY} ${ZWEBRAP_LIBRARY})
