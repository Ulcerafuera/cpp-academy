// Una misma plantilla sirve para int, double o cualquier otro tipo numerico,
// y el concepto Numerico convierte un error de tipos en un mensaje legible.

#include "academy/plantillas.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void una_plantilla_muchos_tipos() {
    std::cout << "-- La misma funcion para varios tipos --\n";

    const std::vector<int> enteros{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const std::vector<double> reales{1.5, -2.25, 3.75, -0.5, 10.0};

    std::cout << "pares de 1..10      = " << academy::plantillas::acumular_si(enteros, [](int v) {
        return v % 2 == 0;
    }) << '\n';
    std::cout << "reales positivos    = "
              << academy::plantillas::acumular_si(reales, [](double v) { return v > 0.0; }) << '\n';
    std::cout << "cuantos > 5         = "
              << academy::plantillas::contar_si(enteros, [](int v) { return v > 5; }) << '\n';
    std::cout << "maximo entero       = " << academy::plantillas::maximo(enteros) << '\n';
    std::cout << "maximo real         = " << academy::plantillas::maximo(reales) << '\n';
}

void expresiones_fold() {
    std::cout << "\n-- Expresiones fold --\n";
    std::cout << "suma_total(1, 2, 3)        = " << academy::plantillas::suma_total(1, 2, 3)
              << '\n';
    std::cout << "suma_total(1, 2.5)         = " << academy::plantillas::suma_total(1, 2.5)
              << "  (el tipo comun es double)\n";
    std::cout << "suma_total(1, 2, 3, 4, 5)  = " << academy::plantillas::suma_total(1, 2, 3, 4, 5)
              << '\n';
}

void que_rechaza_el_concepto() {
    std::cout << "\n-- Lo que el concepto deja fuera --\n";
    std::cout << std::boolalpha;
    std::cout << "Numerico<int>         = " << academy::plantillas::Numerico<int> << '\n';
    std::cout << "Numerico<double>      = " << academy::plantillas::Numerico<double> << '\n';
    std::cout << "Numerico<bool>        = " << academy::plantillas::Numerico<bool> << '\n';
    std::cout << "Numerico<std::string> = " << academy::plantillas::Numerico<std::string> << '\n';

    // Descomenta para ver el error del compilador. Sin conceptos serian
    // paginas de errores dentro de la plantilla; con ellos, una sola linea
    // diciendo que no se satisface Numerico<std::string>.
    //
    // const std::vector<std::string> nombres{"ana", "luis"};
    // academy::plantillas::maximo(nombres);
}

}  // namespace

int main() {
    try {
        una_plantilla_muchos_tipos();
        expresiones_fold();
        que_rechaza_el_concepto();
    } catch (const std::logic_error& error) {
        std::cerr << "\n[pendiente] " << error.what() << '\n'
                  << "Rellena los TODO de lessons/05-plantillas/exercises/ y vuelve a "
                     "compilar.\n";
        return 1;
    }
    return 0;
}
