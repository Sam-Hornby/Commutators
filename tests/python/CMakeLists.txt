
function(add_python_operators_test name)
   add_test(NAME ${name}_python
            COMMAND ${V_PYTHON} -m pytest -s ${CMAKE_CURRENT_SOURCE_DIR}/${name}.py)
endfunction()

add_python_operators_test(BasicTest)

add_python_operators_test(EvaluateTest)
