
function(add_python_operators_test name)
  add_test(NAME ${name}
           COMMAND python -m pytest -s ${operators_SOURCE_DIR}/python/${name}.py)
endfunction()

add_python_operators_test(BasicTest)

