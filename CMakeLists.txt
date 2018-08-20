cmake_minimum_required(VERSION 3.11)
project(signupeoseos)

set(EOSIO_DEPENDENCY "1.1")
set(EOSIO_WASMSDK_DEPENDENCY "1.1")

set(CMAKE_CXX_STANDARD 14)

if(EOSIO_WASMSDK_ROOT STREQUAL "" OR NOT EOSIO_WASMSDK_ROOT)
    set(EOSIO_WASMSDK_ROOT "/usr/local/eosio.wasmsdk")
endif()

list(APPEND CMAKE_MODULE_PATH ${EOSIO_WASMSDK_ROOT}/lib/cmake)
include(EosioWasmToolchain)

add_executable(${CMAKE_PROJECT_NAME}.wasm ${CMAKE_PROJECT_NAME}.hpp ${CMAKE_PROJECT_NAME}.cpp)

if(EOSIO_ROOT STREQUAL "" OR NOT EOSIO_ROOT)
   set(EOSIO_ROOT "/usr/local/eosio")
endif()

set(EOSIOCPP ${EOSIO_ROOT}/bin/eosiocpp)

file(GLOB CONTRACT_HPP_FILE ${CMAKE_PROJECT_NAME}.hpp)
add_custom_target(abi DEPENDS ${CMAKE_PROJECT_NAME}.abi)
add_custom_command(
	OUTPUT ${CMAKE_PROJECT_NAME}.abi
	COMMAND ${EOSIOCPP} -g ${CMAKE_PROJECT_NAME}.abi ${CONTRACT_HPP_FILE}
	DEPENDS ${CMAKE_PROJECT_NAME}.hpp
	)

include(UnitTestsExternalProject.txt)
