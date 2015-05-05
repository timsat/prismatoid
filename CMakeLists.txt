cmake_minimum_required(VERSION 2.8)

project(PRISMATOID)

find_library(libusb usb-1.0 PATHS /usr/lib )
find_library(libboost_system boost_system PATHS /usr/lib )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    add_definitions(-DDEBUG)
endif(${CMAKE_BUILD_TYPE} MATCHES Debug)

add_executable(prismatoid main.cpp Server.cpp LightpackDevice.cpp ApiParser.cpp colorspace_types.cpp)

target_link_libraries(prismatoid ${libusb} ${libboost_system})
