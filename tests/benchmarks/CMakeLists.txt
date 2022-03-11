
function(add_operators_benchmark name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} benchmark::benchmark)
  add_test(NAME ${name}
           COMMAND ${name}
           WORKING_DIRECTORY ${CMAKE_CURRENT_BUILD_DIR})
endfunction()

add_operators_benchmark(EvaluateBenchmark)
