// Una sola interfaz, varias implementaciones, y lo que pasa cuando te olvidas
// de que la copia tambien tiene que ser virtual.

#include "academy/figuras.hpp"

#include <iomanip>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace {

using academy::polimorfismo::Circulo;
using academy::polimorfismo::Figura;
using academy::polimorfismo::Figuras;
using academy::polimorfismo::Rectangulo;
using academy::polimorfismo::Triangulo;

void inventario(const Figuras& figuras) {
    std::cout << "-- Una coleccion, tres tipos distintos --\n";
    std::cout << std::fixed << std::setprecision(3);

    for (const std::unique_ptr<Figura>& figura : figuras) {
        // El codigo de aqui no sabe que tipo hay detras. Es la tabla de
        // funciones virtuales la que decide a que area() se llama.
        std::cout << "  " << std::setw(12) << std::left << figura->nombre()
                  << " area = " << std::setw(8) << std::right << figura->area()
                  << "   perimetro = " << figura->perimetro() << '\n';
    }

    std::cout << "  area total: " << academy::polimorfismo::area_total(figuras) << '\n';
    std::cout << "  la mayor:   " << academy::polimorfismo::mayor(figuras).nombre() << '\n';
}

void copia_polimorfica(const Figuras& figuras) {
    std::cout << "\n-- clonar() conserva el tipo real --\n";

    const Figuras copias = academy::polimorfismo::clonar_todas(figuras);
    for (const std::unique_ptr<Figura>& copia : copias) {
        std::cout << "  copia de " << copia->nombre() << ", area = " << copia->area()
                  << (dynamic_cast<const Triangulo*>(copia.get()) != nullptr
                          ? "   (dynamic_cast a Triangulo: si)"
                          : "")
                  << '\n';
    }

    std::cout << "\n  Por que hace falta clonar() y no basta el constructor de copia:\n"
                 "  desde una `const Figura&` no se sabe que constructor llamar, y\n"
                 "  asignar a un objeto de la clase base rebanaria la parte derivada.\n"
                 "  Por eso Figura declara sus copias como protegidas.\n";
}

}  // namespace

int main() {
    try {
        Figuras figuras;
        figuras.push_back(std::make_unique<Circulo>(2.0));
        figuras.push_back(std::make_unique<Rectangulo>(3.0, 4.0));
        figuras.push_back(std::make_unique<Rectangulo>(5.0, 5.0));
        figuras.push_back(std::make_unique<Triangulo>(3.0, 4.0, 5.0));

        inventario(figuras);
        copia_polimorfica(figuras);
    } catch (const std::logic_error& error) {
        std::cerr << "\n[pendiente] " << error.what() << '\n'
                  << "Rellena los TODO de lessons/08-polimorfismo/exercises/ y vuelve a "
                     "compilar.\n";
        return 1;
    }
    return 0;
}
