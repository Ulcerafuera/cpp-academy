find_package(Threads REQUIRED)

academy_add_lesson(
    NAME concurrencia
    SOURCES paralelo.cpp
    TESTS test_concurrencia.cpp
    EXAMPLES demo_concurrencia.cpp
    LIBRARIES Threads::Threads)
