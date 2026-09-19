#pragma once

#include <cstddef>

/// Leccion 02: punteros, referencias y memoria dinamica "a mano".
///
/// Esta leccion usa `new[]` / `delete[]` a proposito, para ver el problema que
/// RAII (leccion 03) viene a resolver. En codigo real preferiras
/// `std::vector` o `std::unique_ptr`.
namespace academy::memoria {

/// Suma los @p n enteros que empiezan en @p datos.
///
/// @throws std::invalid_argument si @p datos es nulo y @p n > 0.
/// Con @p n == 0 devuelve 0 aunque @p datos sea nulo.
long long suma(const int* datos, std::size_t n);

/// Invierte in situ los @p n enteros que empiezan en @p datos.
///
/// @throws std::invalid_argument si @p datos es nulo y @p n > 0.
void invertir(int* datos, std::size_t n);

/// Reserva un bloque de @p n enteros con los valores 0, 1, ..., n-1.
///
/// El bloque se libera con liberar_secuencia(). Devuelve `nullptr` si
/// @p n == 0 (y en ese caso no hay nada que liberar).
[[nodiscard]] int* reservar_secuencia(std::size_t n);

/// Libera un bloque devuelto por reservar_secuencia(). Acepta `nullptr`.
void liberar_secuencia(int* datos) noexcept;

}  // namespace academy::memoria
