// Quien es el dueno de cada cosa, y que pasa cuando el dueno desaparece.

#include "academy/punteros.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

namespace {

using academy::punteros::Inventario;
using academy::punteros::Nodo;
using academy::punteros::Recurso;

void un_solo_dueno() {
    std::cout << "-- unique_ptr: un unico dueno --\n";
    std::cout << "recursos vivos al empezar: " << Recurso::vivos() << '\n';

    {
        Inventario taller;
        taller.anadir(academy::punteros::crear("martillo"));
        taller.anadir(academy::punteros::crear("sierra"));
        std::cout << "en el taller: " << taller.tam() << ", vivos: " << Recurso::vivos() << '\n';

        // buscar() presta; el taller sigue siendo el dueno.
        if (const Recurso* prestado = taller.buscar("sierra")) {
            std::cout << "prestado: " << prestado->nombre() << " (el taller sigue teniendolo)\n";
        }

        // extraer() cede la propiedad.
        std::unique_ptr<Recurso> mio = taller.extraer("martillo");
        std::cout << "extraido: " << mio->nombre() << ", quedan " << taller.tam()
                  << " en el taller, vivos: " << Recurso::vivos() << '\n';
    }

    std::cout << "al cerrar el ambito, vivos: " << Recurso::vivos() << " (nadie llamo a delete)\n";
}

void propiedad_compartida() {
    std::cout << "\n-- shared_ptr / weak_ptr: el arbol --\n";

    auto raiz = std::make_shared<Nodo>("proyecto");
    auto src = std::make_shared<Nodo>("src");
    auto main_cpp = std::make_shared<Nodo>("main.cpp");

    raiz->anadir_hijo(src);
    src->anadir_hijo(main_cpp);

    std::cout << "ruta de la hoja: " << main_cpp->ruta() << '\n';
    std::cout << "profundidad:     " << main_cpp->profundidad() << '\n';
    std::cout << "duenos de src:   " << src.use_count() << " (la variable local y su padre)\n";
    std::cout << "nodos vivos:     " << Nodo::vivos() << '\n';
}

void el_padre_muere_primero() {
    std::cout << "\n-- Cuando el padre muere antes que el hijo --\n";

    std::shared_ptr<Nodo> huerfano;
    {
        auto raiz = std::make_shared<Nodo>("efimera");
        huerfano = std::make_shared<Nodo>("hijo");
        raiz->anadir_hijo(huerfano);
        std::cout << "dentro del ambito, padre = " << huerfano->padre()->nombre() << '\n';
    }

    // El weak_ptr caduca solo: no queda un puntero colgando.
    std::cout << "fuera del ambito, padre = "
              << (huerfano->padre() == nullptr ? "<ya no existe>" : "???") << '\n';
    std::cout << "nodos vivos: " << Nodo::vivos() << " (el padre se destruyo, el hijo no)\n";
}

}  // namespace

int main() {
    try {
        un_solo_dueno();
        propiedad_compartida();
        el_padre_muere_primero();
    } catch (const std::logic_error& error) {
        std::cerr << "\n[pendiente] " << error.what() << '\n'
                  << "Rellena los TODO de lessons/07-punteros-inteligentes/exercises/ y vuelve "
                     "a compilar.\n";
        return 1;
    }
    return 0;
}
