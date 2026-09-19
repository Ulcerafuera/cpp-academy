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
/// `bool` queda fuera a proposito: `std::integral<bool>` es cierto, pero sumar
/// booleanos casi nunca es lo que se quiere, y dejarlo pasar convierte un error
/// de tipos en un resultado silenciosamente raro. `char` se excluye por lo
/// mismo.
template<typename T>
concept Numerico =
    (std::integral<T> || std::floating_point<T>) && !std::same_as<std::remove_cv_t<T>, bool> &&
    !std::same_as<std::remove_cv_t<T>, char>;

/// Suma los elementos de @p valores que cumplen @p predicado.
///
/// Devuelve `T{}` si ninguno lo cumple o el vector esta vacio.
template<Numerico T, typename Pred>
    requires std::predicate<Pred, const T&>
[[nodiscard]] T acumular_si(const std::vector<T>& valores, Pred predicado) {
    T total{};
    for (const T& valor : valores) {
        if (predicado(valor)) {
            total += valor;
        }
    }
    return total;
}

/// Referencia al mayor elemento de @p valores.
///
/// Ante empates devuelve el primero. La referencia es valida mientras lo sea
/// @p valores.
///
/// @throws std::invalid_argument si @p valores esta vacio.
template<Numerico T>
[[nodiscard]] const T& maximo(const std::vector<T>& valores) {
    if (valores.empty()) {
        throw std::invalid_argument("maximo: el vector esta vacio");
    }

    const T* mayor = &valores.front();
    for (const T& valor : valores) {
        if (valor > *mayor) {
            mayor = &valor;
        }
    }
    return *mayor;
}

/// Suma un numero cualquiera de argumentos, posiblemente de tipos distintos.
///
/// El resultado es del tipo comun a todos ellos: `suma_total(1, 2.5)` devuelve
/// un `double`.
template<Numerico... Ts>
    requires(sizeof...(Ts) > 0)
[[nodiscard]] std::common_type_t<Ts...> suma_total(Ts... valores) {
    // Expresion fold de C++17: se expande a (v1 + v2 + ... + vn).
    return (static_cast<std::common_type_t<Ts...>>(valores) + ...);
}

/// Cuantos elementos de @p valores cumplen @p predicado.
template<Numerico T, typename Pred>
    requires std::predicate<Pred, const T&>
[[nodiscard]] std::size_t contar_si(const std::vector<T>& valores, Pred predicado) {
    std::size_t total = 0;
    for (const T& valor : valores) {
        if (predicado(valor)) {
            ++total;
        }
    }
    return total;
}

}  // namespace academy::plantillas
