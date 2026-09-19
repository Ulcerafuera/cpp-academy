#include "academy/buffer.hpp"

#include <algorithm>
#include <stdexcept>

namespace academy::raii {

// --- Ya implementado: uselo como referencia de estilo -----------------------

Buffer::Buffer(std::size_t n) : datos_(n > 0 ? new int[n]() : nullptr), tam_(n) {
    // `new int[n]()` con los parentesis finales inicializa a cero.
    // Si `new` lanza, el objeto nunca llega a existir y no hay nada que liberar.
}

int& Buffer::en(std::size_t i) {
    if (i >= tam_) {
        throw std::out_of_range("Buffer::en: indice fuera de rango");
    }
    return datos_[i];
}

const int& Buffer::en(std::size_t i) const {
    if (i >= tam_) {
        throw std::out_of_range("Buffer::en: indice fuera de rango");
    }
    return datos_[i];
}

bool operator==(const Buffer& izquierda, const Buffer& derecha) noexcept {
    return izquierda.tam() == derecha.tam() &&
           std::equal(izquierda.datos(), izquierda.datos() + izquierda.tam(), derecha.datos());
}

// --- Te toca: la regla de cinco ---------------------------------------------

Buffer::~Buffer() {
    // TODO(03): libera el bloque con delete[]. Sin esto, cada Buffer que se
    // destruya filtra memoria (compila con ACADEMY_SANITIZERS=ON y lo veras).
}

Buffer::Buffer(const Buffer& otro) {
    // TODO(03): copia PROFUNDA. Reserva un bloque nuevo del tamano de `otro` y
    // copia sus elementos (std::copy te sirve). Si te limitas a copiar el
    // puntero, los dos objetos liberarian el mismo bloque: doble delete.
    (void)otro;
}

Buffer& Buffer::operator=(const Buffer& otro) {
    // TODO(03): asignacion por copia. Tres cosas que no pueden fallar:
    //   1. la autoasignacion (`b = b`) no debe destruir el contenido;
    //   2. hay que liberar el bloque viejo;
    //   3. si la reserva del bloque nuevo lanza, el objeto debe seguir valido.
    // La forma mas facil de cumplir las tres: copy-and-swap.
    (void)otro;
    return *this;
}

Buffer::Buffer(Buffer&& otro) noexcept {
    // TODO(03): roba el puntero y el tamano de `otro`, y dejalo vacio
    // (nullptr / 0) para que su destructor no libere lo que ya no es suyo.
    (void)otro;
}

Buffer& Buffer::operator=(Buffer&& otro) noexcept {
    // TODO(03): igual que el anterior, pero liberando antes lo que ya tenias.
    // Protege tambien la autoasignacion.
    (void)otro;
    return *this;
}

void Buffer::intercambiar(Buffer& otro) noexcept {
    // TODO(03): intercambia `datos_` y `tam_` con los de `otro`.
    // No se copia ni se reserva nada: por eso puede ser noexcept.
    (void)otro;
}

}  // namespace academy::raii
