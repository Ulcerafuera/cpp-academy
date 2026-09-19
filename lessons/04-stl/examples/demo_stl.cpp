// Cuenta las palabras de un texto y muestra el ranking, comparando el bucle a
// mano con los algoritmos de la biblioteca estandar.

#include "academy/texto.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace {

constexpr std::string_view kTexto =
    "En un lugar de la Mancha, de cuyo nombre no quiero acordarme, no ha mucho "
    "tiempo que vivia un hidalgo de los de lanza en astillero, adarga antigua, "
    "rocin flaco y galgo corredor.";

void ranking() {
    std::cout << "-- Palabras mas frecuentes --\n";
    const auto frecuencias = academy::stl::contar_palabras(kTexto);

    for (const auto& [palabra, veces] : academy::stl::top_n(frecuencias, 5)) {
        std::cout << std::setw(12) << palabra << "  "
                  << std::string(static_cast<std::size_t>(veces), '*') << " (" << veces << ")\n";
    }

    const int total = std::accumulate(
        frecuencias.begin(), frecuencias.end(), 0,
        [](int acumulado, const auto& entrada) { return acumulado + entrada.second; });

    std::cout << "\npalabras distintas: " << frecuencias.size() << '\n'
              << "palabras totales:   " << total << '\n';
}

void algoritmos() {
    std::cout << "\n-- Algoritmos sobre un vector --\n";
    std::vector<int> valores{5, 3, 9, 1, 7, 3, 8, 2};

    std::cout << "suma      = " << std::accumulate(valores.begin(), valores.end(), 0) << '\n';
    std::cout << "maximo    = " << *std::max_element(valores.begin(), valores.end()) << '\n';
    std::cout << "mayores>4 = "
              << std::count_if(valores.begin(), valores.end(), [](int v) { return v > 4; }) << '\n';

    // Idioma "erase-remove": remove_if reordena y devuelve el nuevo final;
    // quien borra de verdad es erase.
    valores.erase(std::remove_if(valores.begin(), valores.end(), [](int v) { return v % 2 == 0; }),
                  valores.end());

    std::cout << "sin pares = ";
    for (const int v : valores) {
        std::cout << v << ' ';
    }
    std::cout << '\n';
}

}  // namespace

int main() {
    try {
        ranking();
        algoritmos();
    } catch (const std::logic_error& error) {
        std::cerr << "\n[pendiente] " << error.what() << '\n'
                  << "Rellena los TODO de lessons/04-stl/exercises/ y vuelve a compilar.\n";
        return 1;
    }
    return 0;
}
