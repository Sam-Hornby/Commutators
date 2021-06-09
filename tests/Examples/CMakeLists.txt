add_executable(HarmonicOscilator HarmonicOscilator.cpp)
add_test(NAME "HarmonicOscilator_cpp"
         COMMAND HarmonicOscilator
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})


add_executable(SimpleFermions SimpleFermions.cpp)
add_test(NAME "SimpleFermions_cpp"
         COMMAND SimpleFermions
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})

add_executable(Bogoliubov Bogoliubov.cpp)
add_test(NAME "Bogoliubov_cpp"
        COMMAND Bogoliubov
        WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
