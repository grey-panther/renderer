cmake_minimum_required(VERSION 3.6)
project(Renderer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11") # Обычная компиляция
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ggdb -g3 -pg -O0")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pg -O0") #Для профилятора
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include_directories(libs src)
# TODO Сделать рекурсивный подхват имён компилируемых файлов.
set(SOURCE_FILES main.cpp libs/tgaimage.cpp src/DrawTools.hpp src/DrawTools.cpp src/ObjFormatModel.cpp src/Vec3f.cpp src/ModelFace.cpp src/Vec3i.cpp)
add_executable(Renderer ${SOURCE_FILES})