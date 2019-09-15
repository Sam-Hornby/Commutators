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
target_link_libraries(PrintTests gtest gtest_main)

add_executable(SortTests SortTests.cpp)
add_test(NAME "SortTests"
      COMMAND SortTests
      WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
target_link_libraries(SortTests gtest gtest_main)

#add_executable(CommutatorTests Commutators.cpp)
#add_test(NAME "CommutatorTests"
#      COMMAND CommutatorTests
#      WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
#target_link_libraries(CommutatorTests operators gtest gtest_main)

#add_executable(NumberTests NumberTests.cpp)
#add_test(NAME "NumberTests"
#      COMMAND NumberTests
#      WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
#target_link_libraries(NumberTests operators gtest gtest_main)

#add_executable(SubstitutionTests SubstitutionTests.cpp)
#add_test(NAME "SubstitutionTests"
#         COMMAND SubstitutionTests
#         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
#target_link_libraries(SubstitutionTests operators gtest gtest_main)

#add_executable(PhysicalExamples PhysicalExamples.cpp)
#add_test(NAME "PhysicalExamples"
#         COMMAND PhysicalExamples
#         WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
#target_link_libraries(PhysicalExamples operators gtest gtest_main)
