
function(add_python_operators_test name)
  add_test(NAME ${name}_python
           COMMAND python -m pytest -s ${operators_SOURCE_DIR}/python/${name}.py)
endfunction()

add_python_operators_test(BasicTest)

add_python_operators_test(EvaluateTest)

