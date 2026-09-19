#include "academy/memoria.hpp"

#include <stdexcept>

namespace academy::memoria {

long long suma(const int* /*datos*/, std::size_t /*n*/) {
    // TODO(02): valida el puntero nulo y recorre el bloque.
    // Acumula en long long: la suma de muchos int se sale de int enseguida.
    throw std::logic_error("TODO: implementa academy::memoria::suma");
}

void invertir(int* /*datos*/, std::size_t /*n*/) {
    // TODO(02): intercambia el primero con el ultimo, el segundo con el
    // penultimo... Cuidado: `n` no tiene signo, asi que `n - 1` con n == 0
    // no vale lo que esperas.
    throw std::logic_error("TODO: implementa academy::memoria::invertir");
}

int* reservar_secuencia(std::size_t /*n*/) {
    // TODO(02): reserva con `new int[n]` y rellena con 0, 1, ..., n-1.
    // Devuelve nullptr si n == 0.
    throw std::logic_error("TODO: implementa academy::memoria::reservar_secuencia");
}

void liberar_secuencia(int* /*datos*/) noexcept {
    // TODO(02): libera con `delete[]`. Lo que se reserva con `new[]` se libera
    // con `delete[]`, nunca con `delete` a secas.
}

}  // namespace academy::memoria
