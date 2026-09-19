# 06 · Concurrencia

Hilos, exclusión mutua, variables de condición y reparto de trabajo.

## Qué se practica

- `std::thread` y `join()`.
- `std::mutex` con `std::lock_guard` / `std::unique_lock` (RAII otra vez: el
  cerrojo se suelta solo, también si se lanza una excepción).
- `std::condition_variable` para esperar sin quemar CPU.
- Terminar un productor/consumidor de forma ordenada.
- Evitar carreras de datos por diseño, no a base de candados.

## Interfaz a implementar

`ColaSegura<T>` (en la cabecera, porque es una plantilla):

```cpp
void poner(T valor);                       // encola y notifica
T tomar();                                 // espera hasta que haya algo
std::optional<T> intentar_tomar();         // no bloquea nunca
void cerrar();                             // despierta a todos
```

Y en `paralelo.cpp`:

```cpp
long long suma_paralela(const std::vector<int>& valores, unsigned hilos);
unsigned  hilos_disponibles() noexcept;
```

`cerrada()` y `tam()` ya están escritas como referencia de estilo.

## Trampas que los tests comprueban

1. **Esperar sin predicado.** `wait` puede volver sin que nadie haya notificado
   (*despertar espurio*). Siempre con predicado:
   `cv.wait(cerrojo, [this] { return !cola_.empty() || cerrada_; });`
2. **`notify_one` al cerrar.** Cerrar tiene que despertar a **todos** los
   consumidores (`notify_all`); con `notify_one` los demás se quedan dormidos
   para siempre.
3. **Notificar con el cerrojo cogido.** Funciona, pero el hilo que despierta
   choca inmediatamente con el mutex y se vuelve a dormir. Notifica al salir
   del ámbito del `lock_guard`.
4. **Capturar por referencia en el bucle de hilos.** Si el lambda captura
   `inicio` y `fin` por referencia, todos los hilos acaban viendo el último
   tramo. Captúralos **por valor**.
5. **El reparto desigual.** 7 elementos entre 3 hilos son 3 + 2 + 2, no
   2 + 2 + 2. Los tests comprueban que no se pierde ningún elemento.
6. **Leer los parciales antes del `join`.** Hay que esperar a todos los hilos
   *antes* de sumar.

## Carrera de datos

Un `long long total` compartido con `total += x` desde varios hilos es una
carrera de datos: **comportamiento indefinido**, no "un número un poco mal".
`suma_paralela` lo evita por diseño: cada hilo escribe en *su* posición de un
vector de parciales, así que no hay dos hilos tocando el mismo dato y no hace
falta ni mutex ni atómico.

## Ejecutar

```bash
ctest --preset ejercicios -R "concurrencia"
./build/ejercicios/lessons/06-concurrencia/ejemplo_demo_concurrencia
```

ThreadSanitizer es la herramienta para cazar carreras (no se puede combinar con
AddressSanitizer, por eso no está en el preset `sanitizers`):

```bash
cmake -B build/tsan -DACADEMY_SOLUTIONS=ON \
      -DCMAKE_CXX_FLAGS="-fsanitize=thread -g"
cmake --build build/tsan
ctest --test-dir build/tsan -R "concurrencia"
```
