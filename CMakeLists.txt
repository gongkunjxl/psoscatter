cmake_minimum_required(VERSION 3.8)
project(pso)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp goalFunc.h Particle.h Swarm.h Swarm.cpp Server.h Machine.h)
add_executable(pso ${SOURCE_FILES})