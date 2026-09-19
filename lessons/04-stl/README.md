# 04 · Contenedores y algoritmos

`std::map`, `std::vector`, `std::string_view` y los algoritmos de `<algorithm>`.

## Qué se practica

- Elegir contenedor: `std::map` ordena por clave, `std::vector` es contiguo y
  rápido de recorrer.
- `std::string_view` como parámetro de entrada: no copia ni reserva.
- Algoritmos en vez de bucles a mano: `std::sort`, `std::partial_sort`,
  `std::min`.
- Escribir un comparador que sea un **orden estricto débil**.

## Interfaz a implementar

```cpp
std::map<std::string, int> contar_palabras(std::string_view texto);

std::vector<std::pair<std::string, int>> top_n(
    const std::map<std::string, int>& frecuencias, std::size_t n);
```

Rellena los `TODO` de `exercises/texto.cpp`.

## Trampas que los tests comprueban

1. **La última palabra.** Si el texto no acaba en separador, hay que volcar lo
   que quede acumulado al terminar el bucle.
2. **Separadores consecutivos.** `"uno,,,dos"` son dos palabras, no cuatro con
   dos vacías por el medio.
3. **`std::isalnum` con `char`.** Sus parámetros deben valer `EOF` o caber en un
   `unsigned char`. Un `char` con el bit alto puesto (cualquier byte de UTF-8)
   se convierte a un `int` negativo y el comportamiento es **indefinido**. Hay
   que hacer `static_cast<unsigned char>(c)` antes.
4. **El desempate del comparador.** Si ante frecuencias iguales devuelves
   `true`, el comparador deja de ser un orden estricto débil y `std::sort`
   incurre en comportamiento indefinido (puede leer fuera del rango). Desempata
   siempre por la palabra.

## `std::map::operator[]` inserta

```cpp
++frecuencias[palabra];   // crea la entrada con 0 y luego la incrementa
```

Es cómodo al contar, pero ojo: **también inserta al leer**. Para consultar sin
modificar, usa `.at()` (lanza `std::out_of_range`), `.find()` o `.contains()`.

## `partial_sort` frente a `sort`

Para el top 10 de un millón de palabras, `std::sort` ordena el millón entero.
`std::partial_sort` deja bien colocados solo los 10 primeros: O(n · log k) en
lugar de O(n · log n).

## Ejecutar

```bash
ctest --preset ejercicios -R "stl"
./build/ejercicios/lessons/04-stl/ejemplo_demo_stl
```
