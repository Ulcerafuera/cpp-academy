# Opciones de compilacion compartidas por todos los objetivos de la academia.
#
# Se exponen a traves de la libreria INTERFACE `academy::options`, que cada
# leccion enlaza. Asi los avisos, el estandar y los sanitizers se definen en un
# unico sitio.

include_guard(GLOBAL)

function(academy_setup_options)
    if(TARGET academy_options)
        return()
    endif()

    add_library(academy_options INTERFACE)
    add_library(academy::options ALIAS academy_options)

    target_compile_features(academy_options INTERFACE cxx_std_20)

    if(MSVC)
        target_compile_options(academy_options INTERFACE /W4 /permissive- /utf-8)
        if(ACADEMY_WARNINGS_AS_ERRORS)
            target_compile_options(academy_options INTERFACE /WX)
        endif()
    else()
        target_compile_options(
            academy_options
            INTERFACE -Wall
                      -Wextra
                      -Wpedantic
                      -Wshadow
                      -Wconversion
                      -Wsign-conversion
                      -Wnon-virtual-dtor
                      -Wold-style-cast
                      -Wcast-align
                      -Wunused
                      -Woverloaded-virtual
                      -Wdouble-promotion)
        if(ACADEMY_WARNINGS_AS_ERRORS)
            target_compile_options(academy_options INTERFACE -Werror)
        endif()
    endif()

    if(ACADEMY_SANITIZERS)
        if(MSVC)
            message(WARNING "ACADEMY_SANITIZERS no esta soportado con MSVC; se ignora.")
        else()
            set(_sanitize -fsanitize=address,undefined -fno-omit-frame-pointer
                          -fno-sanitize-recover=undefined)
            target_compile_options(academy_options INTERFACE ${_sanitize} -g)
            target_link_options(academy_options INTERFACE ${_sanitize})
        endif()
    endif()

    # Genera compile_commands.json para clangd / clang-tidy.
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON PARENT_SCOPE)
endfunction()
