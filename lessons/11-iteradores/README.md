# 11 · Iteradores y un contenedor propio

Escribir una `Lista<T>` cuyo iterador la biblioteca estándar reconozca como uno
de los suyos.

Es la lección que cierra el arco: usa la propiedad de la 07 (`unique_ptr` para
encadenar los nodos), la semántica de movimiento de la 03, las plantillas de la
05 y los rangos de la 10.

## La idea

En C++ no te integras con la STL **heredando** de nada. Te integras
**cumpliendo un contrato**:

- el contenedor expone `begin()` y `end()`;
- lo que devuelven declara unos alias (`value_type`, `difference_type`,
  `iterator_category`…) y unas operaciones (`*`, `++`, `==`).

Con eso, y sin tocar una línea de la biblioteca, ya funcionan el `for` de rango,
`std::find`, `std::accumulate` y las vistas de `<ranges>`.

## Qué se practica

- Los alias que hacen que un tipo sea un iterador.
- `operator++` de prefijo y de sufijo, y por qué no son lo mismo.
- Una sola plantilla de iterador para `iterator` y `const_iterator`.
- Una lista enlazada que posee sus nodos con `std::unique_ptr`.

## Lo que hay que implementar

En `exercises/include/academy/lista.hpp`:

| Dónde | Qué |
|---|---|
| `Iterador::operator++()` | avanzar al siguiente nodo |
| `Iterador::operator++(int)` | devolver el valor **anterior** |
| `Iterador::operator==` | comparar los nodos apuntados |
| `Lista::begin()` (×2) | el iterador al primer nodo |
| `Lista::anadir` | enganchar por el final en O(1) |
| `Lista::sacar_primero` | desenganchar por delante |

El bloque de alias del iterador y `limpiar()` ya están escritos: léelos antes de
empezar.

## Trampas que los tests comprueban

1. **`ultimo_` tras vaciar la lista.** Si `sacar_primero` deja la lista vacía y
   no pone `ultimo_` a `nullptr`, el siguiente `anadir` escribe en un nodo ya
   destruido. El test «vaciar del todo y volver a llenar» va justo a por esto.
2. **El sufijo devuelve el valor anterior.** `*(it++)` es el elemento en el que
   estabas; `*(++it)` es el siguiente. Por eso en los bucles se prefiere el de
   prefijo: el de sufijo tiene que hacer una copia.
3. **El iterador observa, no posee.** `nodo_->siguiente` es un `unique_ptr`; hay
   que quedarse con su `.get()`.
4. **El destructor recursivo desborda la pila.** Destruir el primer `unique_ptr`
   dispara `~Nodo` → `~unique_ptr` → `~Nodo`… tan hondo como larga sea la lista.
   Con 100 000 nodos eso revienta. `limpiar()` desmonta la cadena en un bucle;
   está escrito, pero entiende por qué.
5. **Un `forward_iterator` se construye por defecto.** Ese iterador «singular»
   no apunta a nada, y dos de ellos son iguales entre sí.

## Los alias, uno a uno

```cpp
using iterator_category = std::forward_iterator_tag;  // qué sabe hacer
using iterator_concept  = std::forward_iterator_tag;  // lo mismo, para <ranges>
using value_type        = std::remove_const_t<Valor>; // el tipo del elemento
using difference_type   = std::ptrdiff_t;             // distancia entre dos
using pointer           = Valor*;
using reference         = Valor&;
```

Sin ellos, `std::find` no compila: los algoritmos los consultan para saber si
pueden retroceder, saltar o solo avanzar.

`iterator_concept` con `forward_iterator_tag` es lo que promete que **dos
recorridos dan lo mismo** y que los iteradores siguen siendo válidos al copiarse
— justo lo que un iterador de entrada (`input_iterator`) no garantiza.

## Un iterador, dos instanciaciones

`Iterador<T>` es el mutable y `Iterador<const T>` el constante. Una sola
plantilla, cero código duplicado, y una conversión de `iterator` a
`const_iterator` (pero no al revés):

```cpp
operator Iterador<const Valor>() const noexcept
    requires(!std::is_const_v<Valor>)   // se quita cuando Valor ya es const
```

## Ejecutar

```bash
ctest --preset ejercicios -R "iteradores"
./build/ejercicios/lessons/11-iteradores/ejemplo_demo_iteradores
```
