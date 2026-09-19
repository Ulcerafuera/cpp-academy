#pragma once

#include <cstddef>
#include <vector>

namespace academy::concurrencia {

/// Suma @p valores repartiendo el trabajo entre @p hilos hilos.
///
/// El resultado es identico al de la suma secuencial. Si @p hilos es mayor que
/// el numero de elementos se usan menos hilos: no tiene sentido crear un hilo
/// para sumar cero numeros.
///
/// @throws std::invalid_argument si @p hilos == 0.
[[nodiscard]] long long suma_paralela(const std::vector<int>& valores, unsigned hilos);

/// Numero de hilos que el sistema puede ejecutar de verdad a la vez.
/// Devuelve al menos 1, incluso si el sistema no sabe responder.
[[nodiscard]] unsigned hilos_disponibles() noexcept;

}  // namespace academy::concurrencia
