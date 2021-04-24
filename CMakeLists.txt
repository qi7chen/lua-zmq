cmake_minimum_required(VERSION 3.5)
project (luazmq)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(PROJECT_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(PROJECT_SRC_DIR ${PROJECT_ROOT_DIR}/src)


if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_SCL_SECURE_NO_WARNINGS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(UNIX)
    add_definitions(-D__STDC_LIMIT_MACROS)
endif()


set(ALL_SRC_FILES 
    ${PROJECT_SRC_DIR}/lua_zmq.c
)

add_library(luazmq SHARED ${ALL_SRC_FILES} )
target_link_libraries(luazmq ${CONAN_LIBS})
