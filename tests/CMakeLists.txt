#add_executable(PrintTests PrintTests.cpp)
#target_include_directories(PrintTests PUBLIC ${PROJECT_SOURCE_DIR}/lib/operators)
#target_link_libraries(PrintTests gtest gtest_main)

#add_test(PrintTests PrintTests)

include_directories(${operators_SOURCE_DIR}/include/operators)
link_directories(${operators_SOURCE_DIR}/lib/operators)

add_executable(PrintTests PrintTests.cpp)


include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
target_link_libraries(PrintTests operators gtest gtest_main)
