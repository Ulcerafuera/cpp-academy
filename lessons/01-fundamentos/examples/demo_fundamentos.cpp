// Ejecuta:  ./build/lessons/01-fundamentos/ejemplo_demo_fundamentos
//
// Si todavia no has rellenado los TODO de exercises/fundamentos.cpp, el
// programa te lo dira en vez de fallar de forma abrupta.

#include "academy/fundamentos.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace {

void tabla_fizzbuzz() {
    std::cout << "-- FizzBuzz del 1 al 20 --\n";
    for (int n = 1; n <= 20; ++n) {
        std::cout << std::setw(3) << n << " -> " << academy::fundamentos::fizzbuzz(n) << '\n';
    }
}

void tabla_mcd() {
    std::cout << "\n-- Maximo comun divisor --\n";
    const int pares[][2] = {{12, 18}, {1071, 462}, {17, 5}, {0, 9}};
    for (const auto& par : pares) {
        std::cout << "mcd(" << par[0] << ", " << par[1]
                  << ") = " << academy::fundamentos::mcd(par[0], par[1]) << '\n';
    }
}

void tabla_primos() {
    std::cout << "\n-- Primos menores que 50 --\n";
    for (int n = 2; n < 50; ++n) {
        if (academy::fundamentos::es_primo(n)) {
            std::cout << n << ' ';
        }
    }
    std::cout << '\n';
}

}  // namespace

int main() {
    try {
        tabla_fizzbuzz();
        tabla_mcd();
        tabla_primos();
    } catch (const std::logic_error& error) {
        std::cerr << "\n[pendiente] " << error.what() << '\n'
                  << "Rellena los TODO de lessons/01-fundamentos/exercises/ y vuelve a "
                     "compilar.\n";
        return 1;
    }
    return 0;
}
