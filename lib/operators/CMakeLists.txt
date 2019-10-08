
include_directories(${operators_SOURCE_DIR}/include/operators
	            ${operators_SOURCE_DIR}/spdlog/include)

add_library(operators Expression.cpp Operator.cpp)
