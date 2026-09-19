#pragma once

#include <concepts>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <vector>

/// Leccion 05: plantillas, conceptos (C++20) y expresiones fold.
namespace academy::plantillas {

/// Tipos sobre los que tiene sentido hacer aritmetica.
///
/// TODO(05): define el concepto de verdad. Debe aceptar `int`, `long`,
/// `unsigned`, `float` y `double`, y RECHAZAR `bool`, `char`, `std::string` y
/// los punteros.
///
/// Pistas:
///   - `std::integral<T>` y `std::floating_point<T>` estan en <concepts>.
///   - cuidado: `std::integral<bool>` y `std::integral<char>` son CIERTOS, asi
///     que hay que excluirlos aparte con `!std::same_as<T, bool>`.
///   - `std::remove_cv_t<T>` evita que un `const int` se cuele por la rendija.
template<typename T>
concept Numerico = true;

/// Suma los elementos de @p valores que cumplen @p predicado.
///
/// Devuelve `T{}` si ninguno lo cumple o el vector esta vacio.
template<Numerico T, typename Pred>
    requires std::predicate<Pred, const T&>
[[nodiscard]] T acumular_si(const std::vector<T>& valores, Pred predicado) {
    // TODO(05): recorre el vector y acumula los que pasen el predicado.
    // Inicializa el total con `T{}` (0 para los enteros, 0.0 para los reales).
    (void)valores;
    (void)predicado;
    throw std::logic_error("TODO: implementa academy::plantillas::acumular_si");
}

/// Referencia al mayor elemento de @p valores.
///
/// Ante empates devuelve el primero. La referencia es valida mientras lo sea
/// @p valores.
///
/// @throws std::invalid_argument si @p valores esta vacio.
template<Numerico T>
[[nodiscard]] const T& maximo(const std::vector<T>& valores) {
    // TODO(05): valida el vector vacio y busca el mayor.
    // Devuelve una REFERENCIA al elemento, no una copia: guarda un puntero al
    // mejor candidato mientras recorres.
    (void)valores;
    throw std::logic_error("TODO: implementa academy::plantillas::maximo");
}

/// Suma un numero cualquiera de argumentos, posiblemente de tipos distintos.
///
/// El resultado es del tipo comun a todos ellos: `suma_total(1, 2.5)` devuelve
/// un `double`.
template<Numerico... Ts>
    requires(sizeof...(Ts) > 0)
[[nodiscard]] std::common_type_t<Ts...> suma_total(Ts... valores) {
    // TODO(05): usa una expresion fold. La sintaxis es `(expresion + ...)`.
    // Convierte cada valor a `std::common_type_t<Ts...>` antes de sumarlo para
    // que la suma no se haga en el tipo mas estrecho.
    ((void)valores, ...);
    throw std::logic_error("TODO: implementa academy::plantillas::suma_total");
}

/// Cuantos elementos de @p valores cumplen @p predicado.
template<Numerico T, typename Pred>
    requires std::predicate<Pred, const T&>
[[nodiscard]] std::size_t contar_si(const std::vector<T>& valores, Pred predicado) {
    // TODO(05): como acumular_si, pero contando en vez de sumando.
    (void)valores;
    (void)predicado;
    throw std::logic_error("TODO: implementa academy::plantillas::contar_si");
}

}  // namespace academy::plantillas
