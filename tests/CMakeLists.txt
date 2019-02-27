#add_executable(PrintTests PrintTests.cpp)
#target_include_directories(PrintTests PUBLIC ${PROJECT_SOURCE_DIR}/lib/operators)
#target_link_libraries(PrintTests gtest gtest_main)

#add_test(PrintTests PrintTests)

include_directories(${operators_SOURCE_DIR}/include/operators)
link_directories(${operators_SOURCE_DIR}/lib/operators)

add_executable(PrintTests PrintTests.cpp)
add_test(NAME "PrintTests"
      COMMAND PrintTests
      WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})


include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(PrintTests operators gtest gtest_main)

add_executable(SortTests SortTests.cpp)
add_test(NAME "SortTests"
      COMMAND SortTests
      WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
target_link_libraries(SortTests operators gtest gtest_main)

add_executable(CommutatorTests Commutators.cpp)
add_test(NAME "CommutatorTests"
      COMMAND CommutatorTests
      WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
target_link_libraries(CommutatorTests operators gtest gtest_main)
