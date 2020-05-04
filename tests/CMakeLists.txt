macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

find_package(Boost)


include_directories(${operators_SOURCE_DIR}/include/operators
                    ${Boost_INCLUDE_DIRS}
	            ${operators_SOURCE_DIR}/spdlog/include)
link_directories(${operators_SOURCE_DIR}/lib/operators)

add_subdirectory(Examples)

add_executable(PrintTests PrintTests.cpp)
add_test(NAME "PrintTests"
      COMMAND PrintTests
      WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})


add_executable(SortTests SortTests.cpp)
add_test(NAME "SortTests"
      COMMAND SortTests
      WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})

add_executable(CommutatorTests Commutators.cpp)
add_test(NAME "CommutatorTests"
      COMMAND CommutatorTests
      WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})

add_executable(NumberTests NumberTests.cpp)
add_test(NAME "NumberTests"
      COMMAND NumberTests
      WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})

add_executable(SubstitutionTests SubstitutionTests.cpp)
add_test(NAME "SubstitutionTests"
         COMMAND SubstitutionTests
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})

add_executable(TransformTests TransformTests.cpp)
add_test(NAME "TransformTests"
         COMMAND TransformTests
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})

add_executable(LoggingTest LoggingTest.cpp)
add_test(NAME "LoggingTest"
         COMMAND LoggingTest
         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
SET_TESTS_PROPERTIES(LoggingTest
    PROPERTIES ENVIRONMENT "OPERATORS_LOG_LEVEL=TRACE")
