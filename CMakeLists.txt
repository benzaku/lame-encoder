cmake_minimum_required (VERSION 3.18.4)
project (converter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")

add_subdirectory(libmp3lame-CMAKE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libmp3lame-CMAKE/include)

add_executable(converter
    src/main.cpp
    src/Task.h
    src/Task.cpp
    src/Executor.cpp
    src/Executor.h
    src/Mp3Encoder.h
    src/Mp3Encoder.cpp
    src/WavFileReader.h
    src/WavFileReader.cpp
)

target_link_libraries (converter 
    PUBLIC mp3lame)


