#pragma once

#include <cstddef>
#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/// Leccion 04: contenedores y algoritmos de la biblioteca estandar.
namespace academy::stl {

/// Cuenta cuantas veces aparece cada palabra en @p texto.
///
/// Una palabra es una secuencia maxima de caracteres alfanumericos ASCII. Todo
/// lo demas (espacios, signos de puntuacion, saltos de linea y cualquier byte
/// no ASCII) actua como separador. Las palabras se normalizan a minusculas,
/// asi que "Casa", "casa" y "CASA" cuentan como la misma.
///
/// Limitacion deliberada: los acentos y la enye son multibyte en UTF-8, de modo
/// que "accion" y "accion" con tilde no se tratan igual. Procesar texto Unicode
/// de verdad requiere una biblioteca especializada (ICU); aqui interesa el uso
/// de los contenedores.
std::map<std::string, int> contar_palabras(std::string_view texto);

/// Devuelve las @p n palabras mas frecuentes de @p frecuencias.
///
/// El orden es por frecuencia descendente y, a igualdad de frecuencia, por
/// palabra en orden alfabetico ascendente. Si hay menos de @p n palabras, se
/// devuelven todas.
std::vector<std::pair<std::string, int>> top_n(const std::map<std::string, int>& frecuencias,
                                               std::size_t n);

}  // namespace academy::stl
