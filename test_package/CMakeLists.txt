PROJECT(PackageTest)
cmake_minimum_required(VERSION 2.8.12)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
set(CMAKE_CXX_FLAGS "-std=c++20")
conan_basic_setup()

ADD_EXECUTABLE(example src/example.cpp)
TARGET_LINK_LIBRARIES(example ${CONAN_LIBS})
