# cpp-academy

Academia de **C++ moderno (C++20)** por lecciones: cada tema trae una teoría
corta, unos huecos que rellenar y una batería de tests que dice si lo has
hecho bien.

```
lessons/NN-tema/
├── README.md        teoría y pistas del tema
├── include/         la interfaz (no se toca)
├── exercises/       ← aquí escribes tú
├── solutions/       la implementación de referencia
├── tests/           los mismos tests para ambas
└── examples/        programas de demostración
```

La idea: los **mismos tests** validan tu código y el de referencia. Mientras los
`TODO` estén sin rellenar, los tests fallan; cuando pasan todos, la lección está
hecha. Si te atascas, `solutions/` tiene la respuesta comentada.

## Empezar

Necesitas un compilador con C++20 (GCC 11+, Clang 14+ o MSVC 19.30+), CMake 3.21
y conexión a internet la primera vez (se descarga Catch2).

```bash
git clone https://github.com/Ulcerafuera/cpp-academy.git
cd cpp-academy

cmake --preset ejercicios        # configura
cmake --build --preset ejercicios
ctest --preset ejercicios        # 63 tests en rojo: ese es el punto de partida
```

Abre `lessons/01-fundamentos/README.md`, rellena los `TODO` de
`lessons/01-fundamentos/exercises/`, recompila y vuelve a lanzar los tests.

### El ciclo de trabajo

```bash
# compila y lanza solo la lección en la que estás
cmake --build --preset ejercicios && ctest --preset ejercicios -R "fundamentos"

# ejecuta el programa de demostración de la lección
./build/ejercicios/lessons/01-fundamentos/ejemplo_demo_fundamentos

# ¿atascado? mira cómo lo hace la referencia, y compruébalo
cmake --preset soluciones && ctest --preset soluciones -R "fundamentos"
```

## Lecciones

| # | Tema | Qué se construye | Conceptos |
|---|------|------------------|-----------|
| [01](lessons/01-fundamentos/) | Fundamentos | `fizzbuzz`, `mcd`, `es_primo` | tipos, control de flujo, excepciones, desbordamiento |
| [02](lessons/02-memoria/) | Punteros y memoria | suma, inversión y reserva manual | punteros, `new[]`/`delete[]`, aritmética sin signo |
| [03](lessons/03-raii/) | Clases y RAII | un `Buffer` con semántica de valor | regla de cinco, copy-and-swap, `noexcept` |
| [04](lessons/04-stl/) | Contenedores y algoritmos | contador de palabras y ranking | `std::map`, `std::string_view`, `partial_sort` |
| [05](lessons/05-plantillas/) | Plantillas y conceptos | funciones genéricas restringidas | *concepts*, plantillas variádicas, fold |
| [06](lessons/06-concurrencia/) | Concurrencia | cola bloqueante y suma paralela | `std::thread`, mutex, variables de condición |
| [07](lessons/07-punteros-inteligentes/) | Punteros inteligentes | un inventario y un árbol con padres | `unique_ptr`, `shared_ptr`, `weak_ptr`, ciclos |
| [08](lessons/08-polimorfismo/) | Herencia y polimorfismo | jerarquía de figuras planas | interfaces, `override`, destructor virtual, clonación |
| [09](lessons/09-errores/) | Errores y tipos suma | analizador de expresiones | `optional`, `variant`, `visit`, excepciones |
| [10](lessons/10-rangos/) | Rangos y vistas | tuberías de transformación | `views::filter/transform/take/split`, pereza |
| [11](lessons/11-iteradores/) | Iteradores propios | una `Lista<T>` que la STL acepta | `forward_iterator`, alias de iterador, `ranges` |

Las lecciones se apoyan unas en otras y forman un arco:

- **01 → 02 → 03**: de los tipos básicos a la memoria manual, y de ahí a RAII
  como la respuesta al problema que la 02 deja planteado.
- **03 → 07**: escrita la gestión de recursos a mano, la 07 usa la de la
  biblioteca y responde a *¿quién es el dueño?*.
- **07 → 08**: manejar objetos polimórficos necesita punteros con dueño.
- **04 → 10**: los algoritmos clásicos y luego las vistas componibles.
- **03 + 05 + 07 + 10 → 11**: el capstone junta movimiento, plantillas,
  propiedad y rangos en un contenedor propio.

## Presets disponibles

| Preset | Qué compila | Para qué |
|---|---|---|
| `ejercicios` | `exercises/` | el día a día |
| `soluciones` | `solutions/` | consultar la referencia |
| `sanitizers` | `exercises/` + ASan/UBSan | cazar fugas y accesos fuera de rango |
| `release` | `solutions/` optimizado | medir rendimiento |

Sin presets también funciona:

```bash
cmake -S . -B build -DACADEMY_SOLUTIONS=OFF
cmake --build build
ctest --test-dir build
```

### Opciones de CMake

| Opción | Por defecto | Efecto |
|---|---|---|
| `ACADEMY_SOLUTIONS` | `OFF` | `ON` compila `solutions/` en vez de `exercises/` |
| `ACADEMY_SANITIZERS` | `OFF` | activa AddressSanitizer y UBSan |
| `ACADEMY_WARNINGS_AS_ERRORS` | `OFF` | `-Werror` (lo que usa el CI) |
| `ACADEMY_BUILD_EXAMPLES` | `ON` | compila los ejecutables de demostración |
| `BUILD_TESTING` | `ON` | compila y registra los tests |

## Herramientas

**Sanitizers.** La forma más rápida de encontrar un error de memoria:

```bash
cmake --preset sanitizers && cmake --build --preset sanitizers
ctest --preset sanitizers
```

**ThreadSanitizer** para las carreras de datos de la lección 06 (no se combina
con AddressSanitizer):

```bash
cmake -S . -B build/tsan -DCMAKE_CXX_FLAGS="-fsanitize=thread -g"
cmake --build build/tsan && ctest --test-dir build/tsan -R concurrencia
```

**Formato.** El proyecto usa `clang-format` (`.clang-format`, estilo Google con
4 espacios y 100 columnas):

```bash
find lessons -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i
```

**Autocompletado.** Los presets generan `compile_commands.json` en el directorio
de compilación; apunta ahí tu editor para que clangd funcione.

## Integración continua

Cada push ejecuta:

- las **soluciones** con GCC 13 y Clang 18, con los avisos como errores;
- que los **ejercicios compilen** (con los `TODO` sin rellenar, para que quien
  clone el repositorio pueda empezar);
- las soluciones bajo **AddressSanitizer + UBSan**;
- la lección de concurrencia bajo **ThreadSanitizer**;
- la comprobación de **formato**.

## Añadir una lección

1. Copia la estructura de una existente en `lessons/NN-tema/`.
2. Declárala en `lessons/CMakeLists.txt`.
3. Escribe su `CMakeLists.txt`:

```cmake
academy_add_lesson(
    NAME mitema
    SOURCES mitema.cpp          # tiene que existir en exercises/ Y en solutions/
    TESTS test_mitema.cpp
    EXAMPLES demo_mitema.cpp)
```

La función está documentada en `cmake/AcademyLesson.cmake`. Si la lección es solo
de cabeceras (plantillas), omite `SOURCES` y pon la cabecera en
`exercises/include/` y `solutions/include/`.

Regla de oro: **el test tiene que pasar con `solutions/` y fallar con
`exercises/`**. Si pasa con los dos, el ejercicio no comprueba nada.
