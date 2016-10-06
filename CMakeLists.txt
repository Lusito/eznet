cmake_minimum_required(VERSION 2.6)

project(eznet)

#enet library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/enet)
include_directories(SYSTEM
	${CMAKE_CURRENT_SOURCE_DIR}/lib/enet/include
)

include_directories(${PROJECT_SOURCE_DIR}/include)

#project files to compile
file(GLOB_RECURSE SOURCE_FILES
 ${CMAKE_CURRENT_SOURCE_DIR}/include/eznet/*.hpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
 ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
)
add_library(eznet STATIC ${SOURCE_FILES})

#message generator
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
endif()
file(GLOB_RECURSE MESSAGE_GENERATOR_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/message_generator/*.cpp)
add_executable(message_generator ${MESSAGE_GENERATOR_SOURCE_FILES})
