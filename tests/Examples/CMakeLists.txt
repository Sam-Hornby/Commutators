include_directories(${operators_SOURCE_DIR}/include/operators)
link_directories(${operators_SOURCE_DIR}/lib/operators)

add_executable(HarmonicOscilator HarmonicOscilator.cpp)
add_test(NAME "HarmonicOscilator"
         COMMAND HarmonicOscilator
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
target_link_libraries(HarmonicOscilator)


add_executable(SimpleFermions SimpleFermions.cpp)
add_test(NAME "SimpleFermions"
         COMMAND SimpleFermions
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
target_link_libraries(SimpleFermions)
