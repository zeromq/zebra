cmake_minimum_required (VERSION 2.8)

project (zebrajni CXX)
enable_language (C)

find_package (JNI REQUIRED)
find_library (ZMQ_LIBRARY NAMES zmq libzmq REQUIRED)
find_library (ZEBRA_LIBRARY NAMES zebra REQUIRED)

include_directories (${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} src/native/include)

set (zebrajni_sources
    src/main/c/org_zeromq_zebra_XrapMsg.c
    src/main/c/org_zeromq_zebra_ZebHandler.c
)

add_library (zebrajni SHARED ${zebrajni_sources})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -O2")
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

target_link_libraries (zebrajni ${ZMQ_LIBRARY} ${ZEBRA_LIBRARY})
