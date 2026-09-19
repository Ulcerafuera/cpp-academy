# 05 · Plantillas y conceptos

Programación genérica con plantillas, restricciones con *concepts* (C++20) y
expresiones *fold*.

> Esta lección es **solo de cabeceras**, y por eso el código a rellenar está en
> `exercises/include/academy/plantillas.hpp` en lugar de en un `.cpp`: el
> compilador necesita ver el cuerpo de una plantilla en cada sitio donde se
> instancia.

## Qué se practica

- Escribir funciones que valgan para cualquier tipo numérico.
- Restringir una plantilla con un `concept` para que los errores sean legibles.
- Plantillas variádicas y expresiones fold.
- `std::common_type_t` para deducir el tipo del resultado.

## Interfaz a implementar

```cpp
template <typename T> concept Numerico = /* TODO */;

template <Numerico T, typename Pred> T acumular_si(const std::vector<T>&, Pred);
template <Numerico T> const T& maximo(const std::vector<T>&);
template <Numerico... Ts> std::common_type_t<Ts...> suma_total(Ts...);
template <Numerico T, typename Pred> std::size_t contar_si(const std::vector<T>&, Pred);
```

## Trampas que los tests comprueban

1. **`std::integral<bool>` es `true`.** Y `std::integral<char>` también. Si el
   concepto se queda en `std::integral<T> || std::floating_point<T>`, se cuelan
   los dos. Hay que excluirlos explícitamente.
2. **`const int` debe seguir siendo numérico.** `std::remove_cv_t<T>` antes de
   comparar con `std::same_as`.
3. **`maximo` devuelve `const T&`, no `T`.** Los tests comparan direcciones
   (`&maximo(v) == &v[1]`), así que devolver una copia falla.
4. **La suma no se trunca.** `suma_total(2, 0.5)` vale `2.5`, no `2`: hay que
   convertir cada argumento a `std::common_type_t<Ts...>` **antes** de sumarlo.
5. **El empate en `maximo`.** Con `>` estricto se conserva el primero; con `>=`
   se quedaría el último.

## Por qué los conceptos

Sin restricciones, pasarle un `std::vector<std::string>` a `maximo` compila
hasta que falla *dentro* del cuerpo, con páginas de errores sobre `operator>`.
Con el concepto, el error es una línea:

```
error: constraints not satisfied
note: the expression 'std::integral<std::string>' evaluated to false
```

El error aparece en la llamada, no en las tripas de la plantilla.

## Expresiones fold

```cpp
return (static_cast<std::common_type_t<Ts...>>(valores) + ...);
```

Se expande a `(v1 + v2 + ... + vn)`. La restricción `requires(sizeof...(Ts) > 0)`
descarta el paquete vacío, que no tendría tipo común.

## Ejecutar

```bash
ctest --preset ejercicios -R "plantillas"
./build/ejercicios/lessons/05-plantillas/ejemplo_demo_plantillas
```
