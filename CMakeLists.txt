cmake_minimum_required(VERSION 3.5.0)
project(daisychains VERSION 0.1.0 LANGUAGES C CXX)

add_library(daisychains INTERFACE)
target_compile_features(daisychains INTERFACE cxx_std_23)
target_include_directories(daisychains INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include> 
)

#============================================================
# Fetch GoogleTest

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        b514bdc898e2951020cbdca1304b75f5950d1f59
)
FetchContent_MakeAvailable(googletest)

#============================================================
# Tests

enable_testing()

function(dc_add_test target)
  add_executable(${target}_test test/${target}_test.cpp)
  target_compile_features(${target}_test PUBLIC cxx_std_23)
  target_link_libraries(${target}_test daisychains GTest::gtest_main GTest::gmock)
  gtest_add_tests(TARGET ${target}_test)
endfunction()

include(GoogleTest)
dc_add_test(chain)
dc_add_test(cycle)
dc_add_test(drop)
dc_add_test(drop_while)
dc_add_test(elements_equal)
dc_add_test(filter)
dc_add_test(repeat)
dc_add_test(take)
dc_add_test(take_while)
dc_add_test(transform)

