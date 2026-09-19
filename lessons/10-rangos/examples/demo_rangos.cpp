// Las mismas transformaciones, escritas a mano y como tuberia de vistas.

#include "academy/rangos.hpp"

#include <iostream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void imprimir(std::string_view etiqueta, const std::vector<int>& valores) {
    std::cout << "  " << etiqueta << ": ";
    for (const int valor : valores) {
        std::cout << valor << ' ';
    }
    std::cout << '\n';
}

void a_mano_y_con_vistas() {
    std::cout << "-- La misma tarea, dos escrituras --\n";
    const std::vector<int> valores{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // A mano: el "que" se pierde entre el "como".
    std::vector<int> a_mano;
    for (const int valor : valores) {
        if (valor % 2 == 0) {
            a_mano.push_back(valor * valor);
        }
    }
    imprimir("a mano   ", a_mano);
    imprimir("con vistas", academy::rangos::pares_al_cuadrado(valores));
}

void la_pereza_en_accion() {
    std::cout << "\n-- Las vistas son perezosas --\n";

    const std::vector<int> valores{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int evaluaciones = 0;

    auto vista = valores | std::views::filter([&evaluaciones](int valor) {
                     ++evaluaciones;
                     return valor % 2 == 0;
                 }) |
                 std::views::transform([](int valor) { return valor * valor; });

    std::cout << "  tuberia construida, evaluaciones: " << evaluaciones << '\n';

    std::cout << "  los dos primeros: ";
    for (const int valor : vista | std::views::take(2)) {
        std::cout << valor << ' ';
    }
    std::cout << "\n  evaluaciones tras pedir dos: " << evaluaciones
              << "  (no se ha recorrido el vector entero)\n";
}

void un_rango_infinito() {
    std::cout << "\n-- Un rango infinito, gracias a la pereza --\n";

    // iota sin segundo argumento no termina nunca. Solo se puede escribir algo
    // asi porque nada se calcula hasta que se pide.
    auto cuadrados_impares =
        std::views::iota(1) | std::views::filter([](int n) { return n % 2 != 0; }) |
        std::views::transform([](int n) { return n * n; }) | std::views::take(6);

    std::cout << "  primeros 6 cuadrados impares: ";
    for (const int valor : cuadrados_impares) {
        std::cout << valor << ' ';
    }
    std::cout << '\n';
}

void texto() {
    std::cout << "\n-- Texto --\n";

    const std::vector<std::string> trozos = academy::rangos::trocear("uno,dos,,cuatro", ',');
    std::cout << "  trocear(\"uno,dos,,cuatro\", ',') -> " << trozos.size() << " trozos: ";
    for (const std::string& trozo : trozos) {
        std::cout << '"' << trozo << "\" ";
    }
    std::cout << '\n';

    std::cout << "  unir de nuevo: \"" << academy::rangos::unir(trozos, " | ") << "\"\n";
}

void cuidado_con_los_temporales() {
    std::cout << "\n-- El peligro de las vistas: los temporales --\n";
    std::cout << "  Una vista NO es dueña de nada: apunta al rango original.\n"
                 "  Si ese rango es un temporal que muere en la misma linea,\n"
                 "  la vista queda apuntando a memoria liberada:\n\n"
                 "    auto v = hacer_vector() | std::views::filter(...);  // MAL\n\n"
                 "  Guarda el contenedor en una variable con nombre primero.\n"
                 "  (Los rangos del estandar marcan esto como 'dangling' cuando\n"
                 "  pueden, pero no siempre pueden.)\n";
}

}  // namespace

int main() {
    try {
        a_mano_y_con_vistas();
        la_pereza_en_accion();
        un_rango_infinito();
        texto();
        cuidado_con_los_temporales();
    } catch (const std::logic_error& error) {
        std::cerr << "\n[pendiente] " << error.what() << '\n'
                  << "Rellena los TODO de lessons/10-rangos/exercises/ y vuelve a compilar.\n";
        return 1;
    }
    return 0;
}
