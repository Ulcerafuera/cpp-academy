#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

/// Leccion 10: rangos y vistas de C++20.
///
/// Un algoritmo clasico toma dos iteradores; uno de `std::ranges` toma el
/// contenedor entero. Y las VISTAS (`std::views::...`) se encadenan con `|`
/// para describir una transformacion sin ejecutarla todavia:
///
///   auto resultado = valores | std::views::filter(es_par)
///                            | std::views::transform(al_cuadrado);
///
/// Nada se ha calculado aun. La vista es perezosa: cada elemento se filtra y se
/// transforma en el momento en que se recorre, y no se reserva memoria por el
/// camino.
namespace academy::rangos {

/// Los elementos pares de @p valores, elevados al cuadrado y en el mismo orden.
[[nodiscard]] std::vector<int> pares_al_cuadrado(const std::vector<int>& valores);

/// Las palabras de @p palabras con al menos @p minimo caracteres.
[[nodiscard]] std::vector<std::string> palabras_largas(const std::vector<std::string>& palabras,
                                                       std::size_t minimo);

/// Los primeros @p n elementos impares de @p valores, en orden.
/// Si hay menos de @p n, devuelve los que haya.
[[nodiscard]] std::vector<int> primeros_impares(const std::vector<int>& valores, std::size_t n);

/// @p valores en orden descendente, sin repetidos.
[[nodiscard]] std::vector<int> ordenar_sin_repetidos(std::vector<int> valores);

/// Los indices (base 0) de los elementos de @p valores que son negativos.
[[nodiscard]] std::vector<std::size_t> indices_negativos(const std::vector<int>& valores);

/// Une @p palabras separandolas con @p separador. Vacio si no hay palabras.
[[nodiscard]] std::string unir(const std::vector<std::string>& palabras,
                               std::string_view separador);

/// Parte @p texto por cada aparicion de @p separador.
///
/// Los trozos vacios se conservan: "a,,b" da {"a", "", "b"} y "," da {"", ""}.
/// Un texto vacio no tiene trozos: devuelve un vector vacio.
[[nodiscard]] std::vector<std::string> trocear(std::string_view texto, char separador);

/// Suma de los cuadrados de los elementos de @p valores que cumplen
/// `minimo <= valor <= maximo`.
[[nodiscard]] long long suma_cuadrados_en_rango(const std::vector<int>& valores, int minimo,
                                                int maximo);

}  // namespace academy::rangos
