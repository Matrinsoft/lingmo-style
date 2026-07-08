set(CMAKE_AUTOMOC ON)

function(add_lingmo_style_test NAME)
    add_executable(${NAME} ${NAME}.cpp)
    target_link_libraries(${NAME} PRIVATE Lingmo::Style Qt6::Test)
    target_compile_definitions(${NAME} PRIVATE
        DATA_DIR="${CMAKE_SOURCE_DIR}/data"
    )
    set_target_properties(${NAME} PROPERTIES UNITY_BUILD OFF)
    add_test(NAME ${NAME} COMMAND ${NAME})
    set_tests_properties(${NAME} PROPERTIES TIMEOUT 30)
endfunction()

add_lingmo_style_test(tst_Style)
