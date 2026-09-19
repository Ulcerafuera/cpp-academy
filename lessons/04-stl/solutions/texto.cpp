#include "academy/texto.hpp"

#include <algorithm>
#include <cctype>

namespace academy::stl {

namespace {

bool es_alfanumerico(char c) {
    // std::isalnum toma un int que debe valer EOF o caber en un unsigned char.
    // Pasarle un char con el bit alto puesto (un byte de UTF-8, por ejemplo)
    // sin convertirlo antes es comportamiento indefinido.
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

char a_minuscula(char c) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

}  // namespace

std::map<std::string, int> contar_palabras(std::string_view texto) {
    std::map<std::string, int> frecuencias;
    std::string palabra;

    for (const char c : texto) {
        if (es_alfanumerico(c)) {
            palabra.push_back(a_minuscula(c));
        } else if (!palabra.empty()) {
            ++frecuencias[palabra];
            palabra.clear();
        }
    }

    // El texto puede acabar sin separador final.
    if (!palabra.empty()) {
        ++frecuencias[palabra];
    }

    return frecuencias;
}

std::vector<std::pair<std::string, int>> top_n(const std::map<std::string, int>& frecuencias,
                                               std::size_t n) {
    std::vector<std::pair<std::string, int>> ordenadas(frecuencias.begin(), frecuencias.end());

    const std::size_t cuantas = std::min(n, ordenadas.size());

    // Orden estricto debil: primero frecuencia descendente y, a igualdad, la
    // palabra ascendente. Sin el desempate el resultado no seria determinista.
    const auto comparador = [](const std::pair<std::string, int>& izquierda,
                               const std::pair<std::string, int>& derecha) {
        if (izquierda.second != derecha.second) {
            return izquierda.second > derecha.second;
        }
        return izquierda.first < derecha.first;
    };

    // partial_sort solo ordena lo que hace falta: O(n_total * log(cuantas)).
    std::partial_sort(ordenadas.begin(), ordenadas.begin() + static_cast<std::ptrdiff_t>(cuantas),
                      ordenadas.end(), comparador);

    ordenadas.resize(cuantas);
    return ordenadas;
}

}  // namespace academy::stl
