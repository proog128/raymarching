 cmake_minimum_required(VERSION 2.8)

project(RayMarching)

find_package(GLEW)
find_package(SDL)
find_package(OpenGL)
find_package(OpenMP)

if(OPENMP_FOUND)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} ")
  set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_C_FLAGS} ")
endif(OPENMP_FOUND)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

if(MSVC)
    add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
endif(MSVC)

include_directories(${GLEW_INCLUDE_PATH} ${SDL_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR})

file(GLOB FILES *.cpp *.c *.h *.hh)

add_executable(RayMarching
               ${FILES})

target_link_libraries(RayMarching ${GLEW_LIBRARY} ${SDL_LIBRARY} ${OPENGL_LIBRARIES})
