#include "academy/memoria.hpp"

#include <stdexcept>
#include <utility>

namespace academy::memoria {

long long suma(const int* datos, std::size_t n) {
    if (datos == nullptr && n > 0) {
        throw std::invalid_argument("suma: puntero nulo con n > 0");
    }

    long long total = 0;
    for (std::size_t i = 0; i < n; ++i) {
        total += datos[i];
    }
    return total;
}

void invertir(int* datos, std::size_t n) {
    if (datos == nullptr && n > 0) {
        throw std::invalid_argument("invertir: puntero nulo con n > 0");
    }
    if (n < 2) {
        return;
    }

    // Se usan dos indices que se cruzan; con n == 0 nunca se evalua n - 1,
    // que en aritmetica sin signo seria un numero enorme.
    for (std::size_t izquierda = 0, derecha = n - 1; izquierda < derecha; ++izquierda, --derecha) {
        std::swap(datos[izquierda], datos[derecha]);
    }
}

int* reservar_secuencia(std::size_t n) {
    if (n == 0) {
        return nullptr;
    }

    int* datos = new int[n];
    for (std::size_t i = 0; i < n; ++i) {
        datos[i] = static_cast<int>(i);
    }
    return datos;
}

void liberar_secuencia(int* datos) noexcept {
    delete[] datos;
}

}  // namespace academy::memoria
