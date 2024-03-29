macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()

find_package(Boost)

set_directory_properties(PROPERTIES LABELS operators)

include_directories(${operators_SOURCE_DIR}/include/operators
                    ${Boost_INCLUDE_DIRS}
	                ${operators_SOURCE_DIR}/spdlog/include
                    ${operators_SOURCE_DIR}/abseil-cpp
                    ${operators_SOURCE_DIR}/mimalloc/include)

set(TEST_LINK_LIBRARIES
      benchmark::benchmark
      ${LIB_LINK_LIBRARIES})

function(add_operators_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} ${TEST_LINK_LIBRARIES})
  add_test(NAME ${name}_cpp
           COMMAND ${name}
           WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
endfunction()

add_subdirectory(Examples)
add_subdirectory(python)
add_subdirectory(benchmarks)

add_operators_test(PrintTests)

add_operators_test(SortTests)

add_operators_test(Commutators)

add_operators_test(NumberTests)

add_operators_test(SubstitutionTests)

add_operators_test(TransformTests)


add_operators_test(LoggingTest)
SET_TESTS_PROPERTIES(LoggingTest_cpp
    PROPERTIES ENVIRONMENT "OPERATORS_LOG_LEVEL=TRACE")

add_operators_test(BogoliubovTests)

add_operators_test(BogoliubovUnitTest)

add_operators_test(CompositeNumberTests)


