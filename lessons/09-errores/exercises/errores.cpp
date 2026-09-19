#include "academy/errores.hpp"

#include <charconv>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <system_error>

namespace academy::errores {

namespace {

/// Idioma "overloaded": junta varios lambdas en un solo objeto invocable, para
/// darle a std::visit un caso por cada alternativa del variant. Ya esta hecho;
/// lo usaras en describir().
///
///   std::visit(Casos{[](double v) { ... },
///                    [](const Error& e) { ... }},
///              resultado);
template<typename... Ts>
struct Casos : Ts... {
    using Ts::operator()...;
};
template<typename... Ts>
Casos(Ts...) -> Casos<Ts...>;

/// Analiza un numero real completo, sin espacios ni sufijos. Ya implementado:
/// sirve de modelo para a_entero() y lo necesitaras en evaluar().
///
/// El [[maybe_unused]] esta solo para que no avise mientras evaluar() siga sin
/// implementar; puedes quitarlo cuando la uses.
[[maybe_unused]] std::optional<double> a_real(std::string_view texto) {
    if (texto.empty()) {
        return std::nullopt;
    }

    double valor = 0.0;
    const char* const fin = texto.data() + texto.size();
    const auto [siguiente, error] = std::from_chars(texto.data(), fin, valor);

    // Se exige haber consumido TODO el texto: si no, "42abc" colaria como 42.
    if (error != std::errc{} || siguiente != fin) {
        return std::nullopt;
    }
    return valor;
}

}  // namespace

// --- Te toca: std::optional -------------------------------------------------

std::optional<int> a_entero(std::string_view texto) {
    // TODO(09): como a_real, pero con int.
    //
    // std::from_chars devuelve una pareja {ptr, ec}. Hay DOS comprobaciones:
    //   - ec != std::errc{}      -> no era un numero, o no cabe en un int
    //   - ptr != texto.data() + texto.size()  -> sobraron caracteres ("42abc")
    // Si te saltas la segunda, "42abc" se analiza como 42 y el test lo pilla.
    (void)texto;
    throw std::logic_error("TODO: implementa academy::errores::a_entero");
}

std::optional<double> media(const std::vector<int>& valores) {
    // TODO(09): nullopt si esta vacio; si no, la media.
    // Acumula en long long antes de dividir: la suma de muchos int desborda,
    // y dividir dos enteros te daria una division entera.
    (void)valores;
    throw std::logic_error("TODO: implementa academy::errores::media");
}

std::optional<int> primero_mayor_que(const std::vector<int>& valores, int limite) {
    // TODO(09): devuelve el primero que supere el limite, o nullopt.
    (void)valores;
    (void)limite;
    throw std::logic_error("TODO: implementa academy::errores::primero_mayor_que");
}

// --- Te toca: std::variant --------------------------------------------------

Resultado dividir(double numerador, double denominador) {
    // TODO(09): con denominador 0 devuelve Error{"division por cero"}.
    // Fijate en que devolver un double o un Error es lo mismo para el tipo de
    // retorno: el variant se construye solo.
    (void)numerador;
    (void)denominador;
    throw std::logic_error("TODO: implementa academy::errores::dividir");
}

Resultado evaluar(std::string_view expresion) {
    // TODO(09): parte la expresion por los dos espacios, analiza los dos
    // numeros con a_real() y aplica el operador.
    //
    // Mensajes de error que los tests esperan:
    //   "formato invalido: se esperaba '<numero> <operador> <numero>'"
    //   "no es un numero: <lo que fuera>"
    //   "operador desconocido: <lo que fuera>"
    // Para la division, delega en dividir() y devuelve su resultado tal cual.
    (void)expresion;
    throw std::logic_error("TODO: implementa academy::errores::evaluar");
}

double valor_o(const Resultado& resultado, double alternativa) {
    // TODO(09): std::get_if<double>(&resultado) devuelve un puntero al valor
    // si la alternativa activa es esa, y nullptr si no. No lanza, a diferencia
    // de std::get.
    (void)resultado;
    (void)alternativa;
    throw std::logic_error("TODO: implementa academy::errores::valor_o");
}

std::string describir(const Resultado& resultado) {
    // TODO(09): usa std::visit con Casos{...}.
    //   double      -> el numero en texto. Usa std::ostringstream y su
    //                  operator<<: da "2" para 2.0 y "2.5" para 2.5, mientras
    //                  que std::to_string daria "2.000000".
    //   const Error& -> "error: " + el mensaje
    (void)resultado;
    throw std::logic_error("TODO: implementa academy::errores::describir");
}

bool es_valor(const Resultado& resultado) noexcept {
    // TODO(09): std::holds_alternative<double>.
    (void)resultado;
    return false;
}

Recuento resumir(const std::vector<Resultado>& resultados) {
    // TODO(09): suma los valores y cuenta los errores en una sola pasada.
    (void)resultados;
    throw std::logic_error("TODO: implementa academy::errores::resumir");
}

}  // namespace academy::errores
