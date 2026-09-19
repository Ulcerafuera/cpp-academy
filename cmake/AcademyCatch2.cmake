# Localiza Catch2 v3. Si no esta instalado en el sistema, se descarga con
# FetchContent (requiere red la primera vez que configuras el proyecto).

include_guard(GLOBAL)

find_package(Catch2 3 QUIET)

if(NOT Catch2_FOUND)
    message(STATUS "Catch2 v3 no encontrado en el sistema: se descargara con FetchContent")
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.7.1
        GIT_SHALLOW TRUE
        SYSTEM)
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
endif()

include(Catch)
