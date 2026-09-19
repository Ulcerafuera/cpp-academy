// Tres formas de decir "esto no ha salido bien", y por que no son
// intercambiables.

#include "academy/errores.hpp"

#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

using academy::errores::Recuento;
using academy::errores::Resultado;

void ausencia_no_es_error() {
    std::cout << "-- optional: no hay valor, y no pasa nada --\n";

    for (const std::string_view entrada : {"42", "-7", "3.5", "abc", "42abc"}) {
        const std::optional<int> numero = academy::errores::a_entero(entrada);
        std::cout << "  \"" << entrada << "\" -> ";
        if (numero.has_value()) {
            std::cout << *numero << '\n';
        } else {
            std::cout << "<no es un entero>\n";
        }
    }

    const std::vector<int> vacio;
    std::cout << "  media de una lista vacia: " << academy::errores::media(vacio).value_or(0.0)
              << "  (value_or evita tener que preguntar)\n";
}

void el_error_lleva_informacion() {
    std::cout << "\n-- variant: el fallo viene con su explicacion --\n";

    const std::vector<std::string_view> entradas{"2 + 2", "10 / 4", "1 / 0",
                                                 "2 ^ 3", "a + 1",  "hola"};

    std::vector<Resultado> resultados;
    resultados.reserve(entradas.size());
    for (const std::string_view entrada : entradas) {
        Resultado resultado = academy::errores::evaluar(entrada);
        std::cout << "  " << entrada << "  ->  " << academy::errores::describir(resultado) << '\n';
        resultados.push_back(std::move(resultado));
    }

    const Recuento recuento = academy::errores::resumir(resultados);
    std::cout << "\n  suma de los que salieron bien: " << recuento.suma << '\n'
              << "  expresiones con error:          " << recuento.errores << '\n';
}

void cuando_toca_una_excepcion() {
    std::cout << "\n-- excepciones: para lo que de verdad es excepcional --\n";

    // Un optional obligaria a comprobar el resultado en cada llamada, aunque
    // el fallo sea practicamente imposible. Una excepcion deja el camino
    // normal limpio y se propaga sola hasta quien pueda hacer algo.
    try {
        const std::vector<int> valores{1, 2, 3};
        std::cout << "  valores.at(10) ...\n";
        std::cout << valores.at(10) << '\n';
    } catch (const std::out_of_range& error) {
        std::cout << "  capturado: " << error.what() << '\n';
    }

    std::cout << "\n  Regla practica:\n"
                 "    optional   la ausencia es normal y esperable\n"
                 "    variant    el fallo lleva datos y el llamante debe mirarlos\n"
                 "    excepcion  el fallo es raro y quien lo arregla esta mas arriba\n";
}

}  // namespace

int main() {
    try {
        ausencia_no_es_error();
        el_error_lleva_informacion();
        cuando_toca_una_excepcion();
    } catch (const std::logic_error& error) {
        std::cerr << "\n[pendiente] " << error.what() << '\n'
                  << "Rellena los TODO de lessons/09-errores/exercises/ y vuelve a compilar.\n";
        return 1;
    }
    return 0;
}
