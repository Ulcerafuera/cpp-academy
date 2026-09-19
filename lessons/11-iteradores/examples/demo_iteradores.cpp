// Un contenedor propio que la biblioteca estandar acepta como uno mas.

#include "academy/lista.hpp"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <string>

namespace {

using academy::iteradores::Lista;

void lo_basico() {
    std::cout << "-- Una lista enlazada propia --\n";

    Lista<std::string> tareas;
    tareas.anadir("compilar");
    tareas.anadir("pasar los tests");
    tareas.anadir("subir los cambios");

    // El `for` de rango solo necesita begin() y end().
    std::cout << "  pendientes (" << tareas.tam() << "):\n";
    for (const std::string& tarea : tareas) {
        std::cout << "    - " << tarea << '\n';
    }

    std::cout << "  hecho: " << tareas.sacar_primero() << ", quedan " << tareas.tam() << '\n';
}

void con_la_biblioteca_estandar() {
    std::cout << "\n-- Los algoritmos no saben que es una Lista --\n";

    Lista<int> numeros;
    for (const int valor : {3, 1, 4, 1, 5, 9, 2, 6}) {
        numeros.anadir(valor);
    }

    std::cout << "  suma            = " << std::accumulate(numeros.begin(), numeros.end(), 0)
              << '\n';
    std::cout << "  maximo          = " << *std::ranges::max_element(numeros) << '\n';
    std::cout << "  cuantos unos    = " << std::ranges::count(numeros, 1) << '\n';
    std::cout << "  cuantos > 3     = "
              << std::ranges::count_if(numeros, [](int v) { return v > 3; }) << '\n';

    std::cout << "  pares por diez  = ";
    for (const int valor : numeros | std::views::filter([](int v) { return v % 2 == 0; }) |
                               std::views::transform([](int v) { return v * 10; })) {
        std::cout << valor << ' ';
    }
    std::cout << '\n';
}

void lo_que_hace_falta_para_que_funcione() {
    std::cout << "\n-- Que hace falta para llegar hasta aqui --\n";
    std::cout << std::boolalpha;
    std::cout << "  forward_iterator<iterator>        = "
              << std::forward_iterator<Lista<int>::iterator> << '\n';
    std::cout << "  forward_iterator<const_iterator>  = "
              << std::forward_iterator<Lista<int>::const_iterator> << '\n';
    std::cout << "  ranges::forward_range<Lista<int>> = "
              << std::ranges::forward_range<Lista<int>> << '\n';
    std::cout << "\n  No hay herencia de ninguna clase base: basta con exponer\n"
                 "  begin()/end() y con que el iterador declare sus alias y sus\n"
                 "  operaciones. Es un contrato, no una jerarquia.\n";
}

}  // namespace

int main() {
    try {
        lo_basico();
        con_la_biblioteca_estandar();
        lo_que_hace_falta_para_que_funcione();
    } catch (const std::logic_error& error) {
        std::cerr << "\n[pendiente] " << error.what() << '\n'
                  << "Rellena los TODO de lessons/11-iteradores/exercises/ y vuelve a "
                     "compilar.\n";
        return 1;
    }
    return 0;
}
