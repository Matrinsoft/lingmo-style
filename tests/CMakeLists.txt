set(CMAKE_AUTOMOC ON)

function(add_lingmo_style_test NAME)
    add_executable(${NAME} ${NAME}.cpp)
    target_link_libraries(${NAME} PRIVATE LingmoStyle Qt6::Test)
    target_include_directories(${NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/src
    )
    target_compile_definitions(${NAME} PRIVATE
        DATA_DIR="${CMAKE_SOURCE_DIR}/data"
    )
    set_target_properties(${NAME} PROPERTIES UNITY_BUILD OFF)
    add_test(NAME ${NAME} COMMAND ${NAME})
    set_tests_properties(${NAME} PROPERTIES TIMEOUT 30)
endfunction()

add_lingmo_style_test(tst_Style)
add_lingmo_style_test(tst_ColorTable)

# StylePlugin test: compiles the plugin source directly so we can
# test the factory logic without requiring an installed Qt plugin.
add_executable(tst_StylePlugin
    tst_StylePlugin.cpp
    ${CMAKE_SOURCE_DIR}/src/LingmoStyle/plugin/StylePlugin.h
    ${CMAKE_SOURCE_DIR}/src/LingmoStyle/plugin/StylePlugin.cpp
)
target_include_directories(tst_StylePlugin PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/LingmoStyle/plugin
)
target_link_libraries(tst_StylePlugin PRIVATE LingmoStyle Qt6::Test)
set_target_properties(tst_StylePlugin PROPERTIES UNITY_BUILD OFF)
add_test(NAME tst_StylePlugin COMMAND tst_StylePlugin)
set_tests_properties(tst_StylePlugin PROPERTIES TIMEOUT 30)
