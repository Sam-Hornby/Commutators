include_directories(${operators_SOURCE_DIR}/include/operators)
link_directories(${operators_SOURCE_DIR}/lib/operators)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(HarmonicOscilator HarmonicOscilator.cpp)
add_test(NAME "HarmonicOscilator"
         COMMAND HarmonicOscilator
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
target_link_libraries(HarmonicOscilator gtest gtest_main)


add_executable(SimpleFermions SimpleFermions.cpp)
add_test(NAME "SimpleFermions"
         COMMAND SimpleFermions
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
target_link_libraries(SimpleFermions gtest gtest_main)
