macro(lingmo_pch TARGET)
    cmake_parse_arguments(PCH "" "" "PUBLIC;PRIVATE" ${ARGN})
    if(PCH_PUBLIC OR PCH_PRIVATE)
        target_precompile_headers(${TARGET}
            PUBLIC ${PCH_PUBLIC}
            PRIVATE ${PCH_PRIVATE}
        )
    endif()
endmacro()
