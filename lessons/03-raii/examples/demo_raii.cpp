// Muestra cuando se llama a cada una de las cinco operaciones especiales.
//
// La clase Trazador imprime su ciclo de vida; Buffer hace lo mismo de verdad,
// gestionando memoria.

#include "academy/buffer.hpp"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {

class Trazador {
public:
    explicit Trazador(std::string nombre) : nombre_(std::move(nombre)) {
        std::cout << "  constructor      " << nombre_ << '\n';
    }
    Trazador(const Trazador& otro) : nombre_(otro.nombre_ + "-copia") {
        std::cout << "  copia de         " << otro.nombre_ << '\n';
    }
    Trazador(Trazador&& otro) noexcept : nombre_(std::move(otro.nombre_)) {
        std::cout << "  movido           " << nombre_ << '\n';
        otro.nombre_ = "<movido>";
    }
    Trazador& operator=(const Trazador& otro) {
        if (this != &otro) {
            nombre_ = otro.nombre_ + "-asignado";
            std::cout << "  asignado copia   " << nombre_ << '\n';
        }
        return *this;
    }
    Trazador& operator=(Trazador&& otro) noexcept {
        if (this != &otro) {
            nombre_ = std::move(otro.nombre_);
            otro.nombre_ = "<movido>";
            std::cout << "  asignado mov.    " << nombre_ << '\n';
        }
        return *this;
    }
    ~Trazador() { std::cout << "  destructor       " << nombre_ << '\n'; }

private:
    std::string nombre_;
};

void ciclo_de_vida() {
    std::cout << "-- Ciclo de vida --\n";
    Trazador a("a");
    Trazador b(a);             // constructor de copia
    Trazador c(std::move(b));  // constructor de movimiento
    c = a;                     // asignacion por copia
    a = std::move(c);          // asignacion por movimiento
    std::cout << "-- fin del ambito: destructores en orden inverso --\n";
}

void buffers() {
    std::cout << "\n-- Buffer --\n";
    academy::raii::Buffer original(4);
    for (std::size_t i = 0; i < original.tam(); ++i) {
        original[i] = static_cast<int>(i * 10);
    }

    academy::raii::Buffer copia(original);
    copia[0] = -1;

    std::cout << "original[0] = " << original[0] << "  (la copia es profunda)\n";
    std::cout << "copia[0]    = " << copia[0] << '\n';

    const int* bloque = original.datos();
    academy::raii::Buffer movido(std::move(original));
    std::cout << "el bloque no se ha copiado: " << std::boolalpha << (movido.datos() == bloque)
              << '\n';
    std::cout << "el origen queda vacio pero valido: tam = " << original.tam() << '\n';
}

}  // namespace

int main() {
    ciclo_de_vida();
    buffers();
    return 0;
}
