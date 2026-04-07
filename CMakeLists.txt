cmake_minimum_required(VERSION 3.20...3.28)
project(LLVMDefUseGraph LANGUAGES C CXX)

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCE_FILES
    src/DefUseGraph.cpp
    src/GraphTools.cpp
    src/Common.cpp
)

add_llvm_pass_plugin(DefUseGraph ${SOURCE_FILES})

target_include_directories(DefUseGraph PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LLVM_INCLUDE_DIRS}
)

set_target_properties(DefUseGraph PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)
