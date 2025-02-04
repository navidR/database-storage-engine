cmake_minimum_required(VERSION 3.10)
project(storage-engine LANGUAGES CXX)


if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DNDEBUG)
endif()

set(SRC_DIR "src")
set(INC_DIR "inc")
set(SRC_FILES
       "${SRC_DIR}/main.cpp"
       "${SRC_DIR}/pageheader.cpp"
       "${SRC_DIR}/pageheader_test.cpp"
       "${SRC_DIR}/page.cpp"
       "${SRC_DIR}/datapage.cpp"
       "${SRC_DIR}/datapage_test.cpp"
       "${SRC_DIR}/directorypage.cpp"
       "${SRC_DIR}/directorypage_test.cpp"
       "${SRC_DIR}/table.cpp"
       "${SRC_DIR}/table_test.cpp"
       "${SRC_DIR}/delete_test.cpp")
set(INC_FILES
       "${INC_DIR}/pageheader.hpp"
       "${INC_DIR}/pagetype.hpp"
       "${INC_DIR}/page.hpp"
       "${INC_DIR}/datapage.hpp"
       "${INC_DIR}/directorypage.hpp"
       "${INC_DIR}/table.hpp"
       "${INC_DIR}/types.hpp")

set(BLOCK_SIZE 512 CACHE STRING "Block size option for the device." )


## thirdparty libraries

# Adding gflags
message(STATUS "Gflags ...")
add_subdirectory("thirdparty/gflags")

# Adding glog
message(STATUS "GLog ...")
set(WITH_GFLAGS OFF CACHE BOOL "Disable gflags in glog")
add_subdirectory("thirdparty/glog")

# Adding googletest
add_subdirectory("thirdparty/googletest")
enable_testing()


# Adding benchmark
message(STATUS "Googlebenchmark ...")
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable testing in benchmark")
set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "Disable GTest in benchmark")
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Disable installation in benchmark")
add_subdirectory("thirdparty/benchmark")

# Adding hopscotch-map
message(STATUS "robin-map ...")
add_subdirectory("thirdparty/robin-map")


add_executable(${PROJECT_NAME} ${SRC_FILES} ${INC_FILES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE gtest gflags::gflags glog tsl::robin_map benchmark
    )
target_include_directories(${PROJECT_NAME} PRIVATE ${INC_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE DBLOCK_SIZE=${BLOCK_SIZE})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)


add_executable(${PROJECT_NAME}_with_O_DIRECT ${SRC_FILES} ${INC_FILES})
target_link_libraries(${PROJECT_NAME}_with_O_DIRECT
    PRIVATE gtest gflags::gflags glog tsl::robin_map benchmark
    )
target_include_directories(${PROJECT_NAME}_with_O_DIRECT PRIVATE ${INC_DIR})
target_compile_definitions(${PROJECT_NAME}_with_O_DIRECT PRIVATE DBLOCK_SIZE=${BLOCK_SIZE} WITH_O_DIRECT=TRUE)
set_property(TARGET ${PROJECT_NAME}_with_O_DIRECT PROPERTY CXX_STANDARD 17)

