# 01 · Fundamentos

Tipos básicos, control de flujo, funciones y señalización de errores.

## Qué se practica

- Elegir el tipo adecuado y entender cuándo un `int` se queda corto.
- `if` / `for` / `while` y salida temprana de funciones.
- Construir y devolver `std::string`.
- Señalar un error con una excepción (`std::invalid_argument`) en lugar de
  devolver un valor centinela.

## Interfaz a implementar

`include/academy/fundamentos.hpp` fija el contrato; no hace falta tocarlo.

```cpp
std::string fizzbuzz(int n);   // "Fizz", "Buzz", "FizzBuzz" o el número
int         mcd(int a, int b); // algoritmo de Euclides, resultado no negativo
bool        es_primo(int n);   // false para n < 2
```

Rellena los tres `TODO` de `exercises/fundamentos.cpp`.

## Detalles que los tests comprueban

- `fizzbuzz(0)` y `fizzbuzz(-1)` lanzan `std::invalid_argument`.
- `mcd` devuelve siempre un valor `>= 0`, incluso con entradas negativas, y
  `mcd(0, 0) == 0`.
- `es_primo(2147483647)` es `true` y no desborda: comparar
  `divisor <= n / divisor` en lugar de `divisor * divisor <= n` evita el
  desbordamiento de `int`, que es **comportamiento indefinido**.

## Ideas clave

El desbordamiento de enteros con signo no "da la vuelta": es comportamiento
indefinido, y el compilador optimiza asumiendo que nunca ocurre. Cuando un
cálculo intermedio pueda salirse del rango, reordena la expresión (como en la
prueba de primalidad) o usa un tipo más ancho (como en `mcd`).

## Ejecutar

```bash
ctest --preset ejercicios -R "fundamentos"
./build/ejercicios/lessons/01-fundamentos/ejemplo_demo_fundamentos
```
