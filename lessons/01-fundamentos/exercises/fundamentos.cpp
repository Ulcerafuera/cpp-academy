#include "academy/fundamentos.hpp"

#include <stdexcept>

namespace academy::fundamentos {

std::string fizzbuzz(int /*n*/) {
    // TODO(01): recuerda validar n < 1 con std::invalid_argument.
    // Pista: std::to_string convierte el int a std::string.
    throw std::logic_error("TODO: implementa academy::fundamentos::fizzbuzz");
}

int mcd(int /*a*/, int /*b*/) {
    // TODO(01): algoritmo de Euclides.
    //   mientras b != 0:  (a, b) <- (b, a % b)
    // Trabaja con long long por dentro para que los negativos no te sorprendan.
    throw std::logic_error("TODO: implementa academy::fundamentos::mcd");
}

bool es_primo(int /*n*/) {
    // TODO(01): descarta n < 2 y los pares, y prueba divisores impares
    // mientras divisor * divisor <= n.
    throw std::logic_error("TODO: implementa academy::fundamentos::es_primo");
}

}  // namespace academy::fundamentos
