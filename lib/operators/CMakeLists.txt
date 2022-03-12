
include_directories(${operators_SOURCE_DIR}/include/operators
	            			${operators_SOURCE_DIR}/spdlog/include
										${operators_SOURCE_DIR}/abseil-cpp
										${operators_SOURCE_DIR}/mimalloc/include)

add_library(operators Expression.cpp Operator.cpp)

pybind11_add_module(FockOperators PythonModule.cpp)
target_link_libraries(FockOperators PUBLIC ${LIB_LINK_LIBRARIES})
