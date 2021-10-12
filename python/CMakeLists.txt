include(${operators_SOURCE_DIR}/python/CreateVirtualEnv.cmake)


#CreateFockModuleWheel(fock_wheel)
set(FOCK_WHEEL_PATH ${CMAKE_BINARY_DIR}/python/dist/FockModule-0.0.0-cp36-cp36m-linux_x86_64.whl)
add_custom_command(OUTPUT fock_wheel
                   COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/setup.py bdist_wheel
                   DEPENDS FockOperators)


CreateVirtualEnv(operators_venv
                 REQUIREMENTS pytest wheel setuptools ${FOCK_WHEEL_PATH}
                 OUT_PYTHON_EXE V_PYTHON
                 EXTRA_DEPENDS fock_wheel)

add_custom_target(venv_target
                  ALL
                  COMMAND echo venv_target
                  DEPENDS operators_venv fock_wheel)




function(add_python_operators_test name)
  add_test(NAME ${name}_python
           COMMAND ${V_PYTHON} -m pytest -s ${operators_SOURCE_DIR}/python/${name}.py)
endfunction()

add_python_operators_test(BasicTest)

add_python_operators_test(EvaluateTest)

