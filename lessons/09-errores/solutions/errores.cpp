#include "academy/errores.hpp"

#include <charconv>
#include <numeric>
#include <sstream>
#include <system_error>

namespace academy::errores {

namespace {

/// Idioma "overloaded": junta varios lambdas en un solo objeto invocable, para
/// darle a std::visit un caso por cada alternativa del variant.
template<typename... Ts>
struct Casos : Ts... {
    using Ts::operator()...;
};
template<typename... Ts>
Casos(Ts...) -> Casos<Ts...>;

/// Analiza un numero real completo, sin espacios ni sufijos.
std::optional<double> a_real(std::string_view texto) {
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

// --- std::optional ----------------------------------------------------------

std::optional<int> a_entero(std::string_view texto) {
    if (texto.empty()) {
        return std::nullopt;
    }

    int valor = 0;
    const char* const fin = texto.data() + texto.size();
    const auto [siguiente, error] = std::from_chars(texto.data(), fin, valor);

    // errc::result_out_of_range cubre el caso de que no quepa en un int.
    if (error != std::errc{} || siguiente != fin) {
        return std::nullopt;
    }
    return valor;
}

std::optional<double> media(const std::vector<int>& valores) {
    if (valores.empty()) {
        return std::nullopt;
    }

    const long long total = std::accumulate(valores.begin(), valores.end(), 0LL);
    return static_cast<double>(total) / static_cast<double>(valores.size());
}

std::optional<int> primero_mayor_que(const std::vector<int>& valores, int limite) {
    for (const int valor : valores) {
        if (valor > limite) {
            return valor;
        }
    }
    return std::nullopt;
}

// --- std::variant -----------------------------------------------------------

Resultado dividir(double numerador, double denominador) {
    if (denominador == 0.0) {
        return Error{"division por cero"};
    }
    return numerador / denominador;
}

Resultado evaluar(std::string_view expresion) {
    const auto primer_espacio = expresion.find(' ');
    if (primer_espacio == std::string_view::npos) {
        return Error{"formato invalido: se esperaba '<numero> <operador> <numero>'"};
    }

    const auto segundo_espacio = expresion.find(' ', primer_espacio + 1);
    if (segundo_espacio == std::string_view::npos) {
        return Error{"formato invalido: se esperaba '<numero> <operador> <numero>'"};
    }

    const std::string_view izquierda = expresion.substr(0, primer_espacio);
    const std::string_view operador =
        expresion.substr(primer_espacio + 1, segundo_espacio - primer_espacio - 1);
    const std::string_view derecha = expresion.substr(segundo_espacio + 1);

    const std::optional<double> a = a_real(izquierda);
    if (!a.has_value()) {
        return Error{"no es un numero: " + std::string(izquierda)};
    }

    const std::optional<double> b = a_real(derecha);
    if (!b.has_value()) {
        return Error{"no es un numero: " + std::string(derecha)};
    }

    if (operador.size() != 1) {
        return Error{"operador desconocido: " + std::string(operador)};
    }

    switch (operador.front()) {
        case '+':
            return *a + *b;
        case '-':
            return *a - *b;
        case '*':
            return *a * *b;
        case '/':
            return dividir(*a, *b);
        default:
            return Error{"operador desconocido: " + std::string(operador)};
    }
}

double valor_o(const Resultado& resultado, double alternativa) {
    // get_if devuelve un puntero al valor si la alternativa es la pedida, o
    // nullptr si no. A diferencia de std::get, no lanza.
    const double* valor = std::get_if<double>(&resultado);
    return valor != nullptr ? *valor : alternativa;
}

std::string describir(const Resultado& resultado) {
    // std::visit exige un caso para CADA alternativa: si manana se anade una
    // tercera al variant, esto deja de compilar en vez de fallar en ejecucion.
    return std::visit(Casos{[](double valor) {
                                std::ostringstream texto;
                                texto << valor;
                                return texto.str();
                            },
                            [](const Error& error) { return "error: " + error.mensaje; }},
                      resultado);
}

bool es_valor(const Resultado& resultado) noexcept {
    return std::holds_alternative<double>(resultado);
}

Recuento resumir(const std::vector<Resultado>& resultados) {
    Recuento recuento;
    for (const Resultado& resultado : resultados) {
        if (const double* valor = std::get_if<double>(&resultado)) {
            recuento.suma += *valor;
        } else {
            ++recuento.errores;
        }
    }
    return recuento;
}

}  // namespace academy::errores
