cmake_minimum_required(VERSION 3.20...3.28)
project(LLVMDefUseGraph LANGUAGES C CXX)

find_package(LLVM REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_definitions(${LLVM_DEFINITIONS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND "${LLVM_DEFINITIONS}")
add_compile_options(${LLVM_DEFINITIONS_LIST})

set(SOURCE_FILES
    src/def_use_graph.cpp
)

include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
include_directories(include)

add_llvm_pass_plugin(DefUseGraph ${SOURCE_FILES})

llvm_map_components_to_libnames(llvm_libs Core Support Passes IRReader)
target_link_libraries(DefUseGraph PRIVATE ${llvm_libs})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(DefUseGraph PRIVATE -g -O0)
endif()

set_target_properties(DefUseGraph PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)
