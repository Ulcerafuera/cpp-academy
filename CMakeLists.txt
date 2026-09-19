cmake_minimum_required(VERSION 3.21)

project(
    cpp_academy
    VERSION 0.1.0
    DESCRIPTION "Academia de C++ moderno: lecciones, ejercicios y tests"
    LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(CTest)
include(AcademyOptions)
include(AcademyLesson)

# ACADEMY_SOLUTIONS=OFF -> se compila lessons/*/exercises (tu codigo, con los TODO).
# ACADEMY_SOLUTIONS=ON  -> se compila lessons/*/solutions (la referencia).
option(ACADEMY_SOLUTIONS "Compilar las soluciones de referencia en vez de los ejercicios" OFF)
option(ACADEMY_WARNINGS_AS_ERRORS "Tratar los avisos del compilador como errores" OFF)
option(ACADEMY_SANITIZERS "Activar AddressSanitizer y UndefinedBehaviorSanitizer" OFF)
option(ACADEMY_BUILD_EXAMPLES "Compilar los ejemplos ejecutables de cada leccion" ON)

academy_setup_options()

if(BUILD_TESTING)
    include(AcademyCatch2)
endif()

add_subdirectory(lessons)

message(STATUS "")
message(STATUS "== cpp-academy ==")
if(ACADEMY_SOLUTIONS)
    message(STATUS "  modo .............. soluciones (lessons/*/solutions)")
else()
    message(STATUS "  modo .............. ejercicios (lessons/*/exercises)")
endif()
message(STATUS "  estandar .......... C++20")
message(STATUS "  tests ............. ${BUILD_TESTING}")
message(STATUS "  ejemplos .......... ${ACADEMY_BUILD_EXAMPLES}")
message(STATUS "  sanitizers ........ ${ACADEMY_SANITIZERS}")
message(STATUS "  avisos = errores .. ${ACADEMY_WARNINGS_AS_ERRORS}")
message(STATUS "")
