
add_custom_target(fock_wheel
                   ALL
                   COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/create_package.py
                   DEPENDS FockOperators)

set(V_PYTHON ${CMAKE_BINARY_DIR}/operators_venv/bin/python3 CACHE INTERNAL "v_python")
