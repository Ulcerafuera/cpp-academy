#include "academy/paralelo.hpp"

#include <stdexcept>
#include <thread>

namespace academy::concurrencia {

long long suma_paralela(const std::vector<int>& /*valores*/, unsigned /*hilos*/) {
    // TODO(06): reparte el vector en tramos y suma cada uno en un hilo.
    //
    // Guion:
    //   1. valida hilos == 0 con std::invalid_argument y atiende el caso vacio;
    //   2. no crees mas hilos que elementos (std::min);
    //   3. reserva un std::vector<long long> con un hueco POR HILO y que cada
    //      uno escriba solo en el suyo: asi no hay carrera de datos y no
    //      necesitas mutex;
    //   4. reparte el resto de la division entre los primeros hilos para no
    //      dejarte elementos fuera;
    //   5. haz join() de todos ANTES de leer los parciales;
    //   6. suma los parciales.
    //
    // Cuidado con capturar por referencia variables del bucle: captura
    // `inicio` y `fin` POR VALOR, o todos los hilos veran el ultimo tramo.
    throw std::logic_error("TODO: implementa academy::concurrencia::suma_paralela");
}

unsigned hilos_disponibles() noexcept {
    // TODO(06): std::thread::hardware_concurrency() puede devolver 0 cuando el
    // sistema no sabe responder. Devuelve al menos 1.
    return 1U;
}

}  // namespace academy::concurrencia
