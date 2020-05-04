add_executable(HarmonicOscilator HarmonicOscilator.cpp)
add_test(NAME "HarmonicOscilator"
         COMMAND HarmonicOscilator
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})


add_executable(SimpleFermions SimpleFermions.cpp)
add_test(NAME "SimpleFermions"
         COMMAND SimpleFermions
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})

add_executable(Bogoliubov Bogoliubov.cpp)
add_test(NAME "Bogoliubov"
        COMMAND Bogoliubov
        WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
