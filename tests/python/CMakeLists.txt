
function(add_python_operators_test name)
   add_test(NAME ${name}_python
            COMMAND ${V_PYTHON} -m pytest -s ${CMAKE_CURRENT_SOURCE_DIR}/${name}.py)
   set_tests_properties(${name}_python PROPERTIES DEPENDS fock_wheel)
endfunction()

add_python_operators_test(BasicTest)

add_python_operators_test(EvaluateTest)
