# 07 · Punteros inteligentes y propiedad

`std::unique_ptr`, `std::shared_ptr`, `std::weak_ptr` y la pregunta que toda
interfaz en C++ tiene que contestar: **¿quién es el dueño de esto?**

## Qué se practica

- Devolver `std::unique_ptr` para que el tipo diga «te cedo la propiedad».
- Devolver un puntero crudo (`T*`) para decir «te lo presto, no lo destruyas».
- `std::make_unique` / `std::make_shared` en lugar de `new`.
- Romper un ciclo de referencias con `std::weak_ptr`.
- `shared_from_this()` y por qué `std::shared_ptr<T>(this)` está mal.

## El vocabulario de la propiedad

| Tipo | Significado | Coste |
|---|---|---|
| `std::unique_ptr<T>` | un único dueño; mover transfiere | como un puntero crudo |
| `std::shared_ptr<T>` | propiedad compartida, recuento de referencias | dos punteros + un bloque de control atómico |
| `std::weak_ptr<T>` | observa sin alargar la vida | requiere `.lock()` para usarse |
| `T*` / `T&` | prestado, no es dueño de nada | ninguno |

La regla práctica: `unique_ptr` por defecto, `shared_ptr` solo cuando la
propiedad es de verdad compartida y `T*` para observar.

## Interfaz a implementar

```cpp
std::unique_ptr<Recurso> crear(std::string nombre);

class Inventario {
    void anadir(std::unique_ptr<Recurso> recurso);          // toma la propiedad
    Recurso* buscar(std::string_view nombre) noexcept;      // la presta
    std::unique_ptr<Recurso> extraer(std::string_view n);   // la cede
};

class Nodo {
    std::shared_ptr<Nodo> padre() const;                    // .lock() del weak_ptr
    void anadir_hijo(std::shared_ptr<Nodo> hijo);
    std::size_t profundidad() const;
    std::string ruta() const;
};
```

`Recurso::vivos()` y `Nodo::vivos()` ya están escritas: son el instrumento con
el que los tests comprueban que nada se queda sin destruir.

## Trampas que los tests comprueban

1. **`std::shared_ptr<Nodo>(this)` compila y está mal.** Crea un bloque de
   control **nuevo**, independiente del que ya gestiona el objeto: dos recuentos
   que llegan a cero por separado y destruyen el mismo objeto dos veces. La
   respuesta es `shared_from_this()`, que comparte el bloque existente.
2. **El ciclo de referencias.** Si el hijo apuntara al padre con `shared_ptr`,
   padre e hijo se sostendrían mutuamente y su recuento nunca llegaría a cero.
   Es una fuga que ni siquiera se ve como «no se llamó a `delete`»: los
   destructores simplemente no llegan a ejecutarse.
3. **Un `weak_ptr` no se desreferencia.** Hay que pedir `.lock()`, que devuelve
   `nullptr` si el objeto ya murió. Ese es justo el punto: te obliga a
   plantearte qué hacer en ese caso.
4. **`extraer` mueve, no copia.** Un `unique_ptr` no se puede copiar; hay que
   sacarlo del vector con `std::move` y después borrar el hueco.

## `make_unique` en vez de `new`

```cpp
return std::make_unique<Recurso>(nombre);                  // bien
return std::unique_ptr<Recurso>(new Recurso(nombre));      // funciona, pero
```

`make_shared` además reserva el objeto y su bloque de control de una sola vez,
en lugar de dos. Y con `new` suelto en una lista de argumentos, una excepción
en otro argumento puede dejar la memoria huérfana.

## Ejecutar

```bash
ctest --preset ejercicios -R "punteros"
./build/ejercicios/lessons/07-punteros-inteligentes/ejemplo_demo_punteros
```
