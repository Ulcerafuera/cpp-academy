# 02 · Punteros y memoria dinámica

Punteros, referencias, aritmética de punteros y reserva manual con
`new[]` / `delete[]`.

> Esta lección usa memoria manual **a propósito**, para que en la lección 03 se
> entienda qué problema resuelve RAII. En código real usarías `std::vector`.

## Qué se practica

- Un puntero es una dirección: puede ser nulo, y desreferenciarlo entonces es
  comportamiento indefinido.
- Recorrer un bloque con índices `std::size_t` (aritmética **sin signo**).
- Emparejar cada `new[]` con exactamente un `delete[]`.
- Distinguir `const int*` (no puedo escribir) de `int* const` (no puedo
  reapuntar).

## Interfaz a implementar

```cpp
long long suma(const int* datos, std::size_t n);
void      invertir(int* datos, std::size_t n);
int*      reservar_secuencia(std::size_t n);   // 0, 1, ..., n-1
void      liberar_secuencia(int* datos) noexcept;
```

Rellena los `TODO` de `exercises/memoria.cpp`.

## Trampas que los tests comprueban

1. **`n - 1` con `n == 0`.** `std::size_t` no tiene signo: `0 - 1` no es `-1`,
   es `18446744073709551615`. Comprueba el caso vacío *antes* de calcular el
   último índice.
2. **Desbordamiento del acumulador.** Sumar cuatro `INT_MAX` en un `int` es
   comportamiento indefinido; por eso `suma` devuelve `long long`.
3. **`delete` contra `delete[]`.** Liberar con el operador equivocado es
   comportamiento indefinido aunque parezca funcionar.
4. **`n == 0`.** `reservar_secuencia(0)` devuelve `nullptr`, y
   `liberar_secuencia(nullptr)` no hace nada (`delete[] nullptr` es legal).

## Comprobar con sanitizers

La forma rápida de detectar fugas y accesos fuera de rango:

```bash
cmake --preset sanitizers
cmake --build --preset sanitizers
ctest --preset sanitizers -R "memoria"
```

## Ejecutar

```bash
ctest --preset ejercicios -R "memoria"
./build/ejercicios/lessons/02-memoria/ejemplo_demo_memoria
```
