cmake_minimum_required(VERSION 3.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(BUILD_COVERAGE "Build coverage" OFF)
set(
        HUNTER_CACHE_SERVERS
        "https://github.com/bmstu-iu8-cpp-sem-3/hunter-cache"
        CACHE STRING "Default cache server"
)
include("tools/gate/cmake/HunterGate.cmake")

HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.218.tar.gz"
        SHA1 "9a3594a61227967fb92159339ba326701b287b19"
)

project(10_lab)



#find_package(Boost 1.72.0 REQUIRED COMPONENTS program_options coroutine log log_setup thread)
hunter_add_package(GTest)
find_package(GTest CONFIG REQUIRED)


hunter_add_package(Boost COMPONENTS program_options coroutine thread log)
#string(APPEND CMAKE_CXX_FLAGS " -g -Wall -DBOOST_LOG_DYN_LINK")
find_package(Boost 1.70.0 COMPONENTS REQUIRED thread log_setup log program_options coroutine)


hunter_add_package(rocksdb)
find_package(RocksDB CONFIG REQUIRED)

add_library(10_lab STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/sources/source.cpp
        )
#add_executable(10_lab
#        ${CMAKE_CURRENT_SOURCE_DIR}/sources/source.cpp
#        )

add_executable(tests
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/test.cpp
       )

target_include_directories(10_lab
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_include_directories(tests
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

#target_link_libraries(10_lab Boost::log Boost::thread)
#target_link_libraries(10_lab RocksDB::rocksdb Boost::program_options)
#target_link_libraries(tests GTest::main 10_lab)
target_link_libraries(10_lab Boost::log Boost::thread RocksDB::rocksdb Boost::program_options)

enable_testing()
add_test(NAME unit_tests COMMAND tests)

if(BUILD_COVERAGE)
  set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage build." FORCE)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  find_package(codecov)
  add_coverage(10_lab)
  add_coverage(tests)
  list(APPEND LCOV_REMOVE_PATTERNS "'${PROJECT_SOURCE_DIR}/tests/*'")
  coverage_evaluate()
endif()
