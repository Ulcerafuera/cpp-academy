// Muestra la diferencia entre gestionar la memoria a mano y dejar que la
// gestione un objeto. Compila con -DACADEMY_SANITIZERS=ON para ver como
// AddressSanitizer caza las fugas y los accesos fuera de rango.

#include "academy/memoria.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>

namespace {

void a_mano() {
    std::cout << "-- A mano: new[] / delete[] --\n";
    int* datos = academy::memoria::reservar_secuencia(8);

    academy::memoria::invertir(datos, 8);
    std::cout << "suma = " << academy::memoria::suma(datos, 8) << '\n';
    std::cout << "primero = " << datos[0] << ", ultimo = " << datos[7] << '\n';

    // Si algo lanzara una excepcion entre el new[] y este delete[], la memoria
    // se filtraria. Ese es exactamente el problema que resuelve RAII.
    academy::memoria::liberar_secuencia(datos);
}

void con_raii() {
    std::cout << "\n-- Con RAII: std::unique_ptr --\n";
    auto datos = std::make_unique<int[]>(8);
    for (int i = 0; i < 8; ++i) {
        datos[static_cast<std::size_t>(i)] = i * i;
    }

    std::cout << "suma = " << academy::memoria::suma(datos.get(), 8) << '\n';
    // No hay delete[]: el destructor de unique_ptr lo hace, pase lo que pase.
}

}  // namespace

int main() {
    try {
        a_mano();
        con_raii();
    } catch (const std::logic_error& error) {
        std::cerr << "\n[pendiente] " << error.what() << '\n'
                  << "Rellena los TODO de lessons/02-memoria/exercises/ y vuelve a compilar.\n";
        return 1;
    }
    return 0;
}
