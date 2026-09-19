# Leccion solo de cabeceras: el codigo a rellenar vive en
# exercises/include/ y solutions/include/, porque una plantilla no se puede
# separar en un .cpp sin instanciarla explicitamente.
academy_add_lesson(
    NAME plantillas
    TESTS test_plantillas.cpp
    EXAMPLES demo_plantillas.cpp)
