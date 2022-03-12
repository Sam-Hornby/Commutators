
include_directories(${operators_SOURCE_DIR}/include/operators
	            			${operators_SOURCE_DIR}/spdlog/include
										${operators_SOURCE_DIR}/abseil-cpp)

add_library(operators Expression.cpp Operator.cpp)


#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${operators_SOURCE_DIR}/python/FockModule)
pybind11_add_module(FockOperators PythonModule.cpp)
target_link_libraries(FockOperators PUBLIC ${LIB_LINK_LIBRARIES})
