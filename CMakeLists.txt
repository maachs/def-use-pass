cmake_minimum_required(VERSION 3.20...3.28)
project(LLVMDefUseGraph LANGUAGES C CXX)

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

set(LIB_SOURCES
    src/GraphTools.cpp
    src/Common.cpp
)


add_llvm_pass_plugin(DefUseGraph src/DefUseGraph.cpp ${LIB_SOURCES})

target_include_directories(DefUseGraph PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LLVM_INCLUDE_DIRS}
)

llvm_map_components_to_libnames(llvm_libs
    Core
    Support
    IRReader
)

add_executable(unit_tests
    tests/UnitTests.cpp
    ${LIB_SOURCES}
)

target_link_libraries(unit_tests PRIVATE
    GTest::gtest_main
    ${llvm_libs}
)

target_include_directories(unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LLVM_INCLUDE_DIRS}
)

add_test(NAME MyProjectTests COMMAND unit_tests)

file(GLOB_RECURSE ALL_SOURCE_FILES
     "src/*.cpp" "src/*.c"
     "include/*.hpp" "include/*.h")

add_custom_target(format
    COMMAND clang-format -i ${ALL_SOURCE_FILES}
    COMMENT "Formatting code with LLVM style..."
)
