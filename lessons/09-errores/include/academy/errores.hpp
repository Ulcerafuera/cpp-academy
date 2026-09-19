#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

/// Leccion 09: decir "no hay valor" y "algo fue mal" con el sistema de tipos.
///
/// Tres formas de senalar un fallo, y cuando usar cada una:
///
///   std::optional<T>   hay valor o no lo hay, y la ausencia no es un error
///                      (buscar algo que puede no estar)
///   std::variant<...>  el fallo lleva informacion y el llamante DEBE mirarla
///                      (analizar una entrada del usuario)
///   excepciones        el fallo es excepcional y casi nadie puede arreglarlo
///                      aqui mismo (quedarse sin memoria, un invariante roto)
namespace academy::errores {

// --- std::optional ----------------------------------------------------------

/// Convierte @p texto en un entero.
///
/// Acepta un '-' inicial opcional seguido de digitos decimales, sin espacios
/// ni sufijos: "42", "-7" y "0" valen; "", " 42", "+42", "42abc", "abc" y
/// "3.5" devuelven `std::nullopt`. Tambien devuelve `nullopt` si el numero no
/// cabe en un `int`.
[[nodiscard]] std::optional<int> a_entero(std::string_view texto);

/// Media aritmetica de @p valores, o `std::nullopt` si esta vacio.
///
/// La media de una lista vacia no es un error: simplemente no existe.
[[nodiscard]] std::optional<double> media(const std::vector<int>& valores);

/// El primer valor de @p valores estrictamente mayor que @p limite.
[[nodiscard]] std::optional<int> primero_mayor_que(const std::vector<int>& valores, int limite);

// --- std::variant -----------------------------------------------------------

/// Un fallo con mensaje.
struct Error {
    std::string mensaje;

    friend bool operator==(const Error& izquierda, const Error& derecha) {
        return izquierda.mensaje == derecha.mensaje;
    }
};

/// O un numero, o la explicacion de por que no lo hay.
///
/// A diferencia de devolver `-1` o `NaN`, aqui el llamante no puede ignorar el
/// fallo por descuido: para sacar el `double` tiene que preguntar antes.
using Resultado = std::variant<double, Error>;

/// Divide @p numerador entre @p denominador.
/// Con denominador 0 devuelve un Error en vez de lanzar o dar infinito.
[[nodiscard]] Resultado dividir(double numerador, double denominador);

/// Analiza una expresion de la forma "<numero> <operador> <numero>",
/// con los tres separados por un unico espacio y el operador en `+-*/`.
///
/// Ejemplos: "6 / 3" -> 2.0, "2 + 2" -> 4.0, "1 / 0" -> Error,
/// "hola" -> Error, "1 ^ 2" -> Error.
[[nodiscard]] Resultado evaluar(std::string_view expresion);

/// El valor si lo hay, o @p alternativa si es un Error.
[[nodiscard]] double valor_o(const Resultado& resultado, double alternativa);

/// Describe el resultado en texto: "42" o "error: division por cero".
[[nodiscard]] std::string describir(const Resultado& resultado);

/// Cierto si @p resultado lleva un valor.
[[nodiscard]] bool es_valor(const Resultado& resultado) noexcept;

/// Suma los resultados correctos de @p resultados e ignora los errores.
/// Devuelve tambien cuantos errores se encontraron.
struct Recuento {
    double suma = 0.0;
    std::size_t errores = 0;

    friend bool operator==(const Recuento&, const Recuento&) = default;
};

[[nodiscard]] Recuento resumir(const std::vector<Resultado>& resultados);

}  // namespace academy::errores
