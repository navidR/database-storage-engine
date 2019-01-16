cmake_minimum_required(VERSION 2.8)
project(cmpt740-1-storage LANGUAGES CXX)


set(SRC_DIR "src")
set(INC_DIR "inc")
set(SRC_FILES
       "${SRC_DIR}/main.cpp")





add_executable(${PROJECT_NAME} ${SRC_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)

