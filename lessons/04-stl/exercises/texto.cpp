#include "academy/texto.hpp"

#include <stdexcept>

namespace academy::stl {

std::map<std::string, int> contar_palabras(std::string_view /*texto*/) {
    // TODO(04): recorre el texto acumulando los caracteres alfanumericos en una
    // palabra y, al llegar a un separador, incrementa su contador.
    //
    // Pistas:
    //   - `std::map::operator[]` crea la entrada con valor 0 si no existia,
    //     asi que `++frecuencias[palabra]` es todo lo que necesitas.
    //   - `std::isalnum` y `std::tolower` (de <cctype>) toman un `int` que debe
    //     valer EOF o estar en el rango de `unsigned char`: hay que convertir
    //     el `char` con `static_cast<unsigned char>` ANTES de pasarlo, o el
    //     comportamiento es indefinido con bytes no ASCII.
    //   - no te olvides de la ultima palabra si el texto no acaba en separador.
    throw std::logic_error("TODO: implementa academy::stl::contar_palabras");
}

std::vector<std::pair<std::string, int>> top_n(const std::map<std::string, int>& /*frecuencias*/,
                                               std::size_t /*n*/) {
    // TODO(04): vuelca el mapa en un vector de pares, ordenalo y quedate con
    // los n primeros.
    //
    // Pistas:
    //   - `std::partial_sort` o `std::sort` con un comparador propio.
    //   - el comparador debe ser un ORDEN ESTRICTO DEBIL: ante frecuencias
    //     iguales hay que desempatar por la palabra, no devolver `true`.
    //   - `std::min(n, vector.size())` evita quedarte corto o pasarte.
    throw std::logic_error("TODO: implementa academy::stl::top_n");
}

}  // namespace academy::stl
