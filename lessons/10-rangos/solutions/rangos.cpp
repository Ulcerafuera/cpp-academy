#include "academy/rangos.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <numeric>
#include <ranges>

namespace academy::rangos {

namespace {

constexpr auto es_par = [](int valor) { return valor % 2 == 0; };
constexpr auto es_impar = [](int valor) { return valor % 2 != 0; };
constexpr auto al_cuadrado = [](int valor) { return valor * valor; };

}  // namespace

std::vector<int> pares_al_cuadrado(const std::vector<int>& valores) {
    // La tuberia solo DESCRIBE la transformacion: hasta que no se recorre, no
    // se filtra ni se eleva al cuadrado nada, y no se reserva memoria.
    auto vista = valores | std::views::filter(es_par) | std::views::transform(al_cuadrado);

    std::vector<int> resultado;
    std::ranges::copy(vista, std::back_inserter(resultado));
    return resultado;
}

std::vector<std::string> palabras_largas(const std::vector<std::string>& palabras,
                                         std::size_t minimo) {
    auto vista = palabras | std::views::filter([minimo](const std::string& palabra) {
                     return palabra.size() >= minimo;
                 });

    std::vector<std::string> resultado;
    std::ranges::copy(vista, std::back_inserter(resultado));
    return resultado;
}

std::vector<int> primeros_impares(const std::vector<int>& valores, std::size_t n) {
    // take() no se queja si hay menos de n: simplemente para antes.
    auto vista =
        valores | std::views::filter(es_impar) | std::views::take(static_cast<std::ptrdiff_t>(n));

    std::vector<int> resultado;
    std::ranges::copy(vista, std::back_inserter(resultado));
    return resultado;
}

std::vector<int> ordenar_sin_repetidos(std::vector<int> valores) {
    // Los algoritmos de std::ranges toman el contenedor entero en vez de dos
    // iteradores. std::ranges::greater es el comparador "de mayor a menor".
    std::ranges::sort(valores, std::ranges::greater{});

    // unique() no borra: mueve los repetidos al final y devuelve el subrango
    // sobrante. Quien borra de verdad sigue siendo el contenedor.
    const auto sobrantes = std::ranges::unique(valores);
    valores.erase(sobrantes.begin(), sobrantes.end());

    return valores;
}

std::vector<std::size_t> indices_negativos(const std::vector<int>& valores) {
    // views::iota genera los indices sin materializarlos en ningun vector.
    // (views::enumerate, que daria indice y valor a la vez, llega en C++23.)
    auto vista = std::views::iota(std::size_t{0}, valores.size()) |
                 std::views::filter([&valores](std::size_t i) { return valores[i] < 0; });

    std::vector<std::size_t> resultado;
    std::ranges::copy(vista, std::back_inserter(resultado));
    return resultado;
}

std::string unir(const std::vector<std::string>& palabras, std::string_view separador) {
    // Esto seria `palabras | std::views::join_with(separador)`, pero
    // join_with es de C++23. Con C++20 se hace a mano.
    std::string resultado;
    bool primera = true;
    for (const std::string& palabra : palabras) {
        if (!primera) {
            resultado += separador;
        }
        resultado += palabra;
        primera = false;
    }
    return resultado;
}

std::vector<std::string> trocear(std::string_view texto, char separador) {
    std::vector<std::string> trozos;
    for (const auto trozo : std::views::split(texto, separador)) {
        // Cada trozo es una VISTA sobre el texto original, no una copia. Aqui
        // se materializa en un std::string porque el resultado tiene que
        // sobrevivir a `texto`.
        trozos.emplace_back(trozo.begin(), trozo.end());
    }
    return trozos;
}

long long suma_cuadrados_en_rango(const std::vector<int>& valores, int minimo, int maximo) {
    auto vista = valores | std::views::filter([minimo, maximo](int valor) {
                     return valor >= minimo && valor <= maximo;
                 }) |
                 std::views::transform([](int valor) {
                     // Se ensancha ANTES de multiplicar: con int, 50000 * 50000
                     // desborda.
                     const long long ancho = valor;
                     return ancho * ancho;
                 });

    long long total = 0;
    for (const long long cuadrado : vista) {
        total += cuadrado;
    }
    return total;
}

}  // namespace academy::rangos
