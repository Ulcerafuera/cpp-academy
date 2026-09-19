#include "academy/rangos.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <ranges>
#include <stdexcept>

namespace academy::rangos {

namespace {

constexpr auto es_par = [](int valor) { return valor % 2 == 0; };
constexpr auto al_cuadrado = [](int valor) { return valor * valor; };

}  // namespace

// --- Ya implementado: el patron que se repite en toda la leccion ------------

std::vector<int> pares_al_cuadrado(const std::vector<int>& valores) {
    // La tuberia solo DESCRIBE la transformacion: hasta que no se recorre, no
    // se filtra ni se eleva al cuadrado nada, y no se reserva memoria.
    auto vista = valores | std::views::filter(es_par) | std::views::transform(al_cuadrado);

    // C++20 todavia no tiene std::ranges::to (llega en C++23), asi que la
    // vista se materializa copiandola a un vector.
    std::vector<int> resultado;
    std::ranges::copy(vista, std::back_inserter(resultado));
    return resultado;
}

// --- Te toca ----------------------------------------------------------------

std::vector<std::string> palabras_largas(const std::vector<std::string>& palabras,
                                         std::size_t minimo) {
    // TODO(10): filtra las que tengan al menos `minimo` caracteres.
    // El lambda necesita capturar `minimo`.
    (void)palabras;
    (void)minimo;
    throw std::logic_error("TODO: implementa academy::rangos::palabras_largas");
}

std::vector<int> primeros_impares(const std::vector<int>& valores, std::size_t n) {
    // TODO(10): filter para quedarte con los impares, y std::views::take(n)
    // para cortar. take() no se queja si hay menos de n: para antes.
    //
    // Ojo con los negativos: -3 % 2 vale -1, no 1. Compara con != 0.
    // take() espera un entero CON signo, asi que convierte `n`.
    (void)valores;
    (void)n;
    throw std::logic_error("TODO: implementa academy::rangos::primeros_impares");
}

std::vector<int> ordenar_sin_repetidos(std::vector<int> valores) {
    // TODO(10): ordena de mayor a menor y quita los repetidos.
    //
    //   std::ranges::sort(valores, std::ranges::greater{});
    //
    // y despues std::ranges::unique, que NO borra nada: mueve los repetidos al
    // final y te devuelve el subrango sobrante, que tienes que pasarle a
    // valores.erase(). Es el idioma "erase-remove" otra vez.
    (void)valores;
    throw std::logic_error("TODO: implementa academy::rangos::ordenar_sin_repetidos");
}

std::vector<std::size_t> indices_negativos(const std::vector<int>& valores) {
    // TODO(10): genera los indices con
    //   std::views::iota(std::size_t{0}, valores.size())
    // y quedate con aquellos cuyo valor sea negativo.
    // (views::enumerate, que daria indice y valor a la vez, llega en C++23.)
    (void)valores;
    throw std::logic_error("TODO: implementa academy::rangos::indices_negativos");
}

std::string unir(const std::vector<std::string>& palabras, std::string_view separador) {
    // TODO(10): une las palabras con el separador entre medias, sin dejarlo
    // colgando al final.
    //
    // Esto seria `palabras | std::views::join_with(separador)`, pero
    // join_with es de C++23: con C++20 toca hacerlo a mano.
    (void)palabras;
    (void)separador;
    throw std::logic_error("TODO: implementa academy::rangos::unir");
}

std::vector<std::string> trocear(std::string_view texto, char separador) {
    // TODO(10): recorre std::views::split(texto, separador).
    //
    // Cada trozo es una VISTA sobre el texto original, no una copia: hay que
    // materializarlo en un std::string, porque el resultado tiene que
    // sobrevivir a `texto`. Con un trozo `t`, vale emplace_back(t.begin(), t.end()).
    (void)texto;
    (void)separador;
    throw std::logic_error("TODO: implementa academy::rangos::trocear");
}

long long suma_cuadrados_en_rango(const std::vector<int>& valores, int minimo, int maximo) {
    // TODO(10): filtra los que esten en [minimo, maximo] y suma sus cuadrados.
    // Ensancha a long long ANTES de multiplicar: 50000 * 50000 no cabe en int.
    (void)valores;
    (void)minimo;
    (void)maximo;
    throw std::logic_error("TODO: implementa academy::rangos::suma_cuadrados_en_rango");
}

}  // namespace academy::rangos
