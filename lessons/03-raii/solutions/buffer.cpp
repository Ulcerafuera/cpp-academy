#include "academy/buffer.hpp"

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace academy::raii {

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

Buffer::~Buffer() {
    delete[] datos_;
}

Buffer::Buffer(const Buffer& otro)
    : datos_(otro.tam_ > 0 ? new int[otro.tam_] : nullptr), tam_(otro.tam_) {
    std::copy(otro.datos_, otro.datos_ + otro.tam_, datos_);
}

Buffer& Buffer::operator=(const Buffer& otro) {
    // Copy-and-swap: se construye la copia primero (si `new` lanza, *this
    // sigue intacto) y despues se intercambia. La autoasignacion funciona
    // sola, porque se trabaja sobre una copia independiente.
    Buffer copia(otro);
    intercambiar(copia);
    return *this;
    // `copia` se lleva el bloque viejo y lo libera al salir de ambito.
}

Buffer::Buffer(Buffer&& otro) noexcept
    : datos_(std::exchange(otro.datos_, nullptr)), tam_(std::exchange(otro.tam_, 0)) {}

Buffer& Buffer::operator=(Buffer&& otro) noexcept {
    if (this != &otro) {
        delete[] datos_;
        datos_ = std::exchange(otro.datos_, nullptr);
        tam_ = std::exchange(otro.tam_, 0);
    }
    return *this;
}

void Buffer::intercambiar(Buffer& otro) noexcept {
    std::swap(datos_, otro.datos_);
    std::swap(tam_, otro.tam_);
}

}  // namespace academy::raii
