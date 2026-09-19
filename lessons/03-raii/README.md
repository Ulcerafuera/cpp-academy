# 03 · Clases y RAII

La regla de cinco, la semántica de valor y el idioma *copy-and-swap*.

## La idea

**RAII** (*Resource Acquisition Is Initialization*): un recurso —memoria, un
fichero, un mutex— se adquiere en el constructor y se libera en el destructor.
Como el destructor se ejecuta siempre al salir del ámbito, incluso si se lanza
una excepción, el recurso no se puede filtrar.

En la lección 02 había que acordarse de llamar a `liberar_secuencia`. Aquí no
hay nada que recordar: el `Buffer` se limpia solo.

## Qué se practica

Implementar las cinco operaciones especiales de `academy::raii::Buffer`:

| Operación | Firma | Cuidado con |
|---|---|---|
| Destructor | `~Buffer()` | `delete[]`, no `delete` |
| Copia | `Buffer(const Buffer&)` | copia **profunda**, no del puntero |
| Asignación por copia | `operator=(const Buffer&)` | autoasignación, liberar lo viejo, seguridad ante excepciones |
| Movimiento | `Buffer(Buffer&&) noexcept` | dejar el origen vacío **pero válido** |
| Asignación por movimiento | `operator=(Buffer&&) noexcept` | liberar lo viejo, automovida |

Rellena los `TODO` de `exercises/buffer.cpp`.

## La regla de cinco (y la regla de cero)

Si tienes que escribir una de las cinco, casi siempre necesitas las cinco: el
compilador genera versiones por defecto que copian el puntero *tal cual*, y eso
acaba en un **doble `delete`**.

La **regla de cero** es la conclusión práctica: en código real no escribas
ninguna, guarda el recurso en un `std::vector` o un `std::unique_ptr` y deja que
el compilador genere todo correctamente. Este ejercicio es para entender qué
hace por ti.

## Copy-and-swap

La asignación por copia tiene que resolver tres problemas a la vez
(autoasignación, liberar lo antiguo, no dejar el objeto roto si `new` lanza). El
idioma los resuelve los tres de golpe:

```cpp
Buffer& Buffer::operator=(const Buffer& otro) {
    Buffer copia(otro);       // si new lanza aqui, *this sigue intacto
    intercambiar(copia);      // noexcept
    return *this;
}                             // `copia` se lleva el bloque viejo y lo libera
```

Por eso `intercambiar` es `noexcept`: intercambiar dos punteros no puede fallar.

## `noexcept` en el movimiento importa

`std::vector` solo mueve sus elementos al reubicarse si el constructor de
movimiento es `noexcept`; si no, los **copia** para poder garantizar que la
operación es reversible ante una excepción. Un `noexcept` que falta se traduce
en copias silenciosas.

## Ejecutar

```bash
ctest --preset ejercicios -R "raii"
./build/ejercicios/lessons/03-raii/ejemplo_demo_raii
```

Para detectar fugas y dobles liberaciones mientras trabajas:

```bash
ctest --preset sanitizers -R "raii"
```
