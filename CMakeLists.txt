cmake_minimum_required(VERSION 3.5.0)
project(daisychains VERSION 0.1.0 LANGUAGES C CXX)

add_library(daisychains INTERFACE)
target_compile_features(daisychains INTERFACE cxx_std_23)
target_include_directories(daisychains INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include> 
)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  incomplete_chain_test
  test/incomplete_chain_test.cpp
)
target_compile_features(incomplete_chain_test PUBLIC cxx_std_23)
target_link_libraries(
  incomplete_chain_test
  daisychains
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(incomplete_chain_test)

