# 10 · Rangos y vistas

Los algoritmos de `std::ranges` y las vistas componibles de C++20.

## Qué cambia respecto a `<algorithm>`

```cpp
std::sort(v.begin(), v.end());        // clásico: dos iteradores
std::ranges::sort(v);                 // rangos: el contenedor entero
```

Y las **vistas** se encadenan con `|`:

```cpp
auto resultado = valores | std::views::filter(es_par)
                         | std::views::transform(al_cuadrado);
```

Esa línea no calcula nada todavía. Describe la transformación; el trabajo ocurre
cuando alguien recorre el resultado, elemento a elemento, sin reservar ningún
vector intermedio.

## Qué se practica

- `views::filter`, `views::transform`, `views::take`, `views::iota`,
  `views::split`.
- `std::ranges::sort`, `unique`, `copy`.
- Materializar una vista en un contenedor.
- Entender la pereza y, sobre todo, **quién es dueño de qué**.

## Interfaz a implementar

`pares_al_cuadrado` está hecha como modelo. Te tocan las demás:

```cpp
std::vector<std::string> palabras_largas(const std::vector<std::string>&, std::size_t minimo);
std::vector<int>         primeros_impares(const std::vector<int>&, std::size_t n);
std::vector<int>         ordenar_sin_repetidos(std::vector<int> valores);
std::vector<std::size_t> indices_negativos(const std::vector<int>&);
std::string              unir(const std::vector<std::string>&, std::string_view separador);
std::vector<std::string> trocear(std::string_view texto, char separador);
long long                suma_cuadrados_en_rango(const std::vector<int>&, int min, int max);
```

## Trampas que los tests comprueban

1. **`-3 % 2` vale `-1`, no `1`.** Para detectar impares hay que comparar con
   `!= 0`, no con `== 1`, o los negativos se quedan fuera.
2. **`ranges::unique` no borra.** Mueve los repetidos al final y devuelve el
   subrango sobrante; quien borra es `valores.erase(...)`. Y solo elimina
   repetidos **adyacentes**: hay que ordenar antes.
3. **Los trozos de `views::split` son vistas.** No copias. Si el resultado tiene
   que sobrevivir al texto de origen, hay que materializarlos en `std::string`.
4. **`views::split("")` no da un trozo vacío: no da ninguno.** Pero `","` sí da
   dos trozos vacíos.
5. **Ensanchar antes de multiplicar.** `50000 * 50000` desborda un `int`; hay
   que pasar a `long long` **dentro** del `transform`, antes del producto.
6. **`views::take` espera un entero con signo.** Convertir desde `std::size_t`
   explícitamente.

## Lo que C++20 todavía no tiene

| Quieres | C++20 | C++23 |
|---|---|---|
| volcar una vista a un contenedor | `ranges::copy` + `back_inserter` | `ranges::to<std::vector>()` |
| índice y valor a la vez | `views::iota` + indexar | `views::enumerate` |
| unir con separador | a mano | `views::join_with` |

## El peligro: una vista no es dueña de nada

```cpp
auto vista = hacer_vector() | std::views::filter(es_par);   // MAL
```

El vector temporal muere al acabar la sentencia y la vista queda apuntando a
memoria liberada. Guarda el contenedor en una variable con nombre antes de
construir la tubería. La biblioteca detecta algunos de estos casos y los marca
como `dangling`, pero no todos.

## Ejecutar

```bash
ctest --preset ejercicios -R "rangos"
./build/ejercicios/lessons/10-rangos/ejemplo_demo_rangos
```
