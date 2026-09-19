# academy_add_lesson(): declara una leccion de la academia.
#
#   academy_add_lesson(
#       NAME      fundamentos                 # sufijo de los objetivos
#       SOURCES   fundamentos.cpp             # rutas relativas a exercises/ y a solutions/
#       TESTS     test_fundamentos.cpp        # rutas relativas a tests/
#       EXAMPLES  demo_fizzbuzz.cpp           # rutas relativas a examples/
#       LIBRARIES Threads::Threads)           # dependencias extra de la libreria
#
# Cada leccion existe por duplicado: `exercises/` contiene los huecos que
# rellenas tu y `solutions/` la implementacion de referencia. La opcion
# ACADEMY_SOLUTIONS decide cual de las dos carpetas se compila, de modo que los
# MISMOS tests validan ambas.
#
# Objetivos creados:
#   lesson_<NAME>    libreria estatica con la implementacion
#   test_<NAME>      ejecutable de tests (registrado en CTest)
#   ejemplo_<NAME>_<fichero>   un ejecutable por cada EXAMPLES

include_guard(GLOBAL)

function(academy_add_lesson)
    set(options)
    set(one_value_args NAME)
    set(multi_value_args SOURCES TESTS EXAMPLES LIBRARIES)
    cmake_parse_arguments(ARG "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    if(NOT ARG_NAME)
        message(FATAL_ERROR "academy_add_lesson: falta el argumento NAME")
    endif()
    if(ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "academy_add_lesson(${ARG_NAME}): argumentos desconocidos: "
                            "${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if(ACADEMY_SOLUTIONS)
        set(impl_dir "${CMAKE_CURRENT_SOURCE_DIR}/solutions")
    else()
        set(impl_dir "${CMAKE_CURRENT_SOURCE_DIR}/exercises")
    endif()

    if(NOT IS_DIRECTORY "${impl_dir}")
        message(FATAL_ERROR "academy_add_lesson(${ARG_NAME}): no existe ${impl_dir}")
    endif()

    set(lib_target "lesson_${ARG_NAME}")
    set(impl_sources "")
    foreach(src IN LISTS ARG_SOURCES)
        if(NOT EXISTS "${impl_dir}/${src}")
            message(FATAL_ERROR "academy_add_lesson(${ARG_NAME}): falta ${impl_dir}/${src}")
        endif()
        list(APPEND impl_sources "${impl_dir}/${src}")
    endforeach()

    # Una leccion puede ser solo de cabeceras (plantillas). CMake necesita al
    # menos una unidad de traduccion para una libreria estatica, asi que
    # generamos una vacia en lugar de cambiar el tipo de objetivo: de este modo
    # todas las lecciones se enlazan igual (PUBLIC) y las reglas son uniformes.
    if(NOT impl_sources)
        set(placeholder "${CMAKE_CURRENT_BINARY_DIR}/${lib_target}_placeholder.cpp")
        file(
            WRITE "${placeholder}"
            "// Generado por academy_add_lesson: esta leccion es solo de cabeceras.\n"
            "namespace {\n[[maybe_unused]] constexpr int academy_placeholder = 0;\n}\n")
        list(APPEND impl_sources "${placeholder}")
    endif()

    add_library(${lib_target} STATIC ${impl_sources})
    add_library(academy::${ARG_NAME} ALIAS ${lib_target})
    target_link_libraries(${lib_target} PUBLIC academy::options ${ARG_LIBRARIES})

    # Las lecciones 01-04 exponen su cabecera en include/ (interfaz fija que no
    # se toca). Las de plantillas la tienen dentro de exercises/ y solutions/,
    # porque ahi el codigo a rellenar vive en la propia cabecera.
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include")
        target_include_directories(${lib_target} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
    endif()
    if(IS_DIRECTORY "${impl_dir}/include")
        target_include_directories(${lib_target} PUBLIC "${impl_dir}/include")
    endif()

    if(ARG_TESTS AND BUILD_TESTING)
        set(test_target "test_${ARG_NAME}")
        set(test_sources "")
        foreach(src IN LISTS ARG_TESTS)
            if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/${src}")
                message(FATAL_ERROR "academy_add_lesson(${ARG_NAME}): falta tests/${src}")
            endif()
            list(APPEND test_sources "${CMAKE_CURRENT_SOURCE_DIR}/tests/${src}")
        endforeach()

        add_executable(${test_target} ${test_sources})
        target_link_libraries(${test_target} PRIVATE ${lib_target} Catch2::Catch2WithMain)
        catch_discover_tests(${test_target} TEST_PREFIX "${ARG_NAME}: " PROPERTIES TIMEOUT 120)
    endif()

    if(ARG_EXAMPLES AND ACADEMY_BUILD_EXAMPLES)
        foreach(src IN LISTS ARG_EXAMPLES)
            if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples/${src}")
                message(FATAL_ERROR "academy_add_lesson(${ARG_NAME}): falta examples/${src}")
            endif()
            cmake_path(GET src STEM example_name)
            add_executable(ejemplo_${example_name} "${CMAKE_CURRENT_SOURCE_DIR}/examples/${src}")
            target_link_libraries(ejemplo_${example_name} PRIVATE ${lib_target})
        endforeach()
    endif()
endfunction()
