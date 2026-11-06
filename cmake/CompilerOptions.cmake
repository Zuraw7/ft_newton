function(set_project_warnings target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Werror
            -Wshadow
            -Wconversion
            -Wsign-conversion
            -Wdouble-promotion
            -Wformat=2
            -Wformat-signedness
            -Wundef
            -Wcast-align
        )
    endif()
endfunction()
