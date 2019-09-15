
include_directories(${operators_SOURCE_DIR}/include/operators)

add_library(operators Expression.cpp Operator.cpp)
