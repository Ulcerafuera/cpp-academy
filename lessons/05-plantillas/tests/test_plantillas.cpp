#include "academy/plantillas.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

using academy::plantillas::acumular_si;
using academy::plantillas::contar_si;
using academy::plantillas::maximo;
using academy::plantillas::Numerico;
using academy::plantillas::suma_total;

namespace {

/// Cierto si `maximo` acepta un `std::vector<T>`, es decir, si el concepto
/// Numerico deja pasar a T. Permite comprobar las restricciones sin que el
/// test deje de compilar cuando el concepto todavia no esta definido.
template<typename T>
concept AceptadoPorMaximo = requires(const std::vector<T>& valores) { maximo(valores); };

constexpr auto es_par = [](int v) { return v % 2 == 0; };
constexpr auto es_positivo = [](int v) { return v > 0; };
constexpr auto es_positivo_largo = [](long v) { return v > 0; };

}  // namespace

TEST_CASE("el concepto Numerico acepta los tipos aritmeticos", "[plantillas][concepto]") {
    SECTION("enteros") {
        CHECK(Numerico<int>);
        CHECK(Numerico<long>);
        CHECK(Numerico<long long>);
        CHECK(Numerico<short>);
        CHECK(Numerico<unsigned>);
        CHECK(Numerico<std::size_t>);
    }

    SECTION("reales") {
        CHECK(Numerico<float>);
        CHECK(Numerico<double>);
    }

    SECTION("la constancia no cambia nada") {
        CHECK(Numerico<const int>);
        CHECK(Numerico<const double>);
    }
}

TEST_CASE("el concepto Numerico rechaza lo que no es aritmetica", "[plantillas][concepto]") {
    SECTION("bool y char son integrales, pero no queremos sumarlos") {
        CHECK_FALSE(Numerico<bool>);
        CHECK_FALSE(Numerico<char>);
        CHECK_FALSE(Numerico<const bool>);
    }

    SECTION("tipos que no son numeros") {
        CHECK_FALSE(Numerico<std::string>);
        CHECK_FALSE(Numerico<int*>);
        CHECK_FALSE(Numerico<void*>);
        CHECK_FALSE(Numerico<std::vector<int>>);
    }

    SECTION("la restriccion llega a las funciones") {
        CHECK(AceptadoPorMaximo<int>);
        CHECK(AceptadoPorMaximo<double>);
        CHECK_FALSE(AceptadoPorMaximo<bool>);
        CHECK_FALSE(AceptadoPorMaximo<std::string>);
    }
}

TEST_CASE("acumular_si suma solo lo que pasa el filtro", "[plantillas][acumular]") {
    const std::vector<int> valores{1, 2, 3, 4, 5, 6};

    SECTION("con un predicado que filtra") {
        CHECK(acumular_si(valores, es_par) == 12);  // 2 + 4 + 6
        CHECK(acumular_si(valores, es_positivo) == 21);
    }

    SECTION("si no pasa ninguno devuelve el valor neutro") {
        CHECK(acumular_si(valores, [](int v) { return v > 100; }) == 0);
    }

    SECTION("vector vacio") {
        CHECK(acumular_si(std::vector<int>{}, es_par) == 0);
    }

    SECTION("funciona con reales") {
        const std::vector<double> reales{1.5, -2.0, 3.25, -0.5};
        CHECK_THAT(acumular_si(reales, [](double v) { return v > 0.0; }),
                   Catch::Matchers::WithinRel(4.75, 1e-12));
    }

    SECTION("el tipo devuelto es el del vector") {
        STATIC_REQUIRE(
            std::is_same_v<decltype(acumular_si(std::vector<long>{}, es_positivo_largo)), long>);
    }
}

TEST_CASE("maximo devuelve una referencia al mayor elemento", "[plantillas][maximo]") {
    SECTION("caso normal") {
        const std::vector<int> valores{3, 9, 2, 7};
        CHECK(maximo(valores) == 9);
    }

    SECTION("un solo elemento") {
        const std::vector<int> valores{42};
        CHECK(maximo(valores) == 42);
    }

    SECTION("todos negativos") {
        const std::vector<int> valores{-5, -1, -9};
        CHECK(maximo(valores) == -1);
    }

    SECTION("devuelve una referencia al elemento, no una copia") {
        const std::vector<int> valores{1, 5, 3};
        STATIC_REQUIRE(std::is_same_v<decltype(maximo(valores)), const int&>);
        CHECK(&maximo(valores) == &valores[1]);
    }

    SECTION("ante un empate devuelve el primero") {
        const std::vector<int> valores{4, 7, 7, 2};
        CHECK(&maximo(valores) == &valores[1]);
    }

    SECTION("un vector vacio es un error") {
        CHECK_THROWS_AS(maximo(std::vector<int>{}), std::invalid_argument);
        CHECK_THROWS_AS(maximo(std::vector<double>{}), std::invalid_argument);
    }
}

TEST_CASE("suma_total acepta cualquier numero de argumentos", "[plantillas][fold]") {
    SECTION("un solo argumento") {
        CHECK(suma_total(7) == 7);
    }

    SECTION("varios del mismo tipo") {
        CHECK(suma_total(1, 2, 3) == 6);
        CHECK(suma_total(1, 2, 3, 4, 5, 6, 7, 8, 9, 10) == 55);
    }

    SECTION("tipos mezclados: el resultado es el tipo comun") {
        STATIC_REQUIRE(std::is_same_v<decltype(suma_total(1, 2.5)), double>);
        CHECK_THAT(suma_total(1, 2.5), Catch::Matchers::WithinRel(3.5, 1e-12));
        CHECK_THAT(suma_total(1, 2.5F, 3.0), Catch::Matchers::WithinRel(6.5, 1e-12));
    }

    SECTION("la suma no se trunca al tipo mas estrecho") {
        // Si se sumara en int, 2 + 0.5 daria 2.
        CHECK_THAT(suma_total(2, 0.5), Catch::Matchers::WithinRel(2.5, 1e-12));
    }
}

TEST_CASE("contar_si cuenta los elementos que pasan el filtro", "[plantillas][contar]") {
    const std::vector<int> valores{1, 2, 3, 4, 5, 6};

    CHECK(contar_si(valores, es_par) == 3);
    CHECK(contar_si(valores, es_positivo) == 6);
    CHECK(contar_si(valores, [](int v) { return v > 100; }) == 0);
    CHECK(contar_si(std::vector<int>{}, es_par) == 0);

    STATIC_REQUIRE(std::is_same_v<decltype(contar_si(valores, es_par)), std::size_t>);
}
