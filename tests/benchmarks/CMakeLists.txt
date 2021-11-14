
function(add_operators_benchmark name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} benchmark::benchmark)
endfunction()

add_operators_benchmark(EvaluateBenchmark)
