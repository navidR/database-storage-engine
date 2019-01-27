cmake_minimum_required(VERSION 3.10)
project(cmpt740-1-storage LANGUAGES CXX)


set(SRC_DIR "src")
set(INC_DIR "inc")
set(SRC_FILES
       "${SRC_DIR}/main.cpp")


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
message(STATUS "Hopscotch-map ...")
add_subdirectory("thirdparty/hopscotch-map")


add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME}
    PRIVATE gtest gflags::gflags glog tsl::hopscotch_map benchmark
    )
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

