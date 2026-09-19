#include "academy/rangos.hpp"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <ranges>
#include <string>
#include <vector>

using academy::rangos::indices_negativos;
using academy::rangos::ordenar_sin_repetidos;
using academy::rangos::palabras_largas;
using academy::rangos::pares_al_cuadrado;
using academy::rangos::primeros_impares;
using academy::rangos::suma_cuadrados_en_rango;
using academy::rangos::trocear;
using academy::rangos::unir;

using Enteros = std::vector<int>;
using Textos = std::vector<std::string>;
using Indices = std::vector<std::size_t>;

TEST_CASE("pares_al_cuadrado filtra y transforma", "[rangos][filter][transform]") {
    CHECK(pares_al_cuadrado({1, 2, 3, 4, 5, 6}) == Enteros{4, 16, 36});
    CHECK(pares_al_cuadrado({1, 3, 5}) == Enteros{});
    CHECK(pares_al_cuadrado({}) == Enteros{});
    CHECK(pares_al_cuadrado({0}) == Enteros{0});
    CHECK(pares_al_cuadrado({-2, -3, 4}) == Enteros{4, 16});
}

TEST_CASE("palabras_largas filtra por longitud", "[rangos][filter]") {
    const Textos palabras{"a", "casa", "sol", "ordenador", "no"};

    CHECK(palabras_largas(palabras, 4) == Textos{"casa", "ordenador"});
    CHECK(palabras_largas(palabras, 3) == Textos{"casa", "sol", "ordenador"});
    CHECK(palabras_largas(palabras, 100) == Textos{});

    SECTION("con minimo 0 pasan todas, incluida la vacia") {
        const Textos con_vacia{"", "a"};
        CHECK(palabras_largas(con_vacia, 0) == con_vacia);
    }

    SECTION("el limite es 'al menos', no 'mas que'") {
        CHECK(palabras_largas({"abc"}, 3) == Textos{"abc"});
        CHECK(palabras_largas({"abc"}, 4) == Textos{});
    }

    SECTION("se conserva el orden original") {
        CHECK(palabras_largas({"zzzz", "aaaa"}, 4) == Textos{"zzzz", "aaaa"});
    }
}

TEST_CASE("primeros_impares corta la tuberia", "[rangos][take]") {
    const Enteros valores{1, 2, 3, 4, 5, 6, 7};

    CHECK(primeros_impares(valores, 2) == Enteros{1, 3});
    CHECK(primeros_impares(valores, 4) == Enteros{1, 3, 5, 7});
    CHECK(primeros_impares(valores, 0) == Enteros{});

    SECTION("pedir mas de los que hay devuelve todos") {
        CHECK(primeros_impares(valores, 100) == Enteros{1, 3, 5, 7});
    }

    SECTION("los impares negativos tambien cuentan") {
        // -3 % 2 vale -1, no 1: comparar con == 1 los dejaria fuera.
        CHECK(primeros_impares({-3, -2, -1}, 5) == Enteros{-3, -1});
    }

    SECTION("sin impares") {
        CHECK(primeros_impares({2, 4, 6}, 3) == Enteros{});
    }
}

TEST_CASE("ordenar_sin_repetidos ordena de mayor a menor", "[rangos][sort][unique]") {
    CHECK(ordenar_sin_repetidos({3, 1, 4, 1, 5, 9, 2, 6, 5}) == Enteros{9, 6, 5, 4, 3, 2, 1});
    CHECK(ordenar_sin_repetidos({1, 1, 1}) == Enteros{1});
    CHECK(ordenar_sin_repetidos({}) == Enteros{});
    CHECK(ordenar_sin_repetidos({5}) == Enteros{5});

    SECTION("con negativos") {
        CHECK(ordenar_sin_repetidos({-1, 3, -1, 0}) == Enteros{3, 0, -1});
    }

    SECTION("ya ordenado") {
        CHECK(ordenar_sin_repetidos({3, 2, 1}) == Enteros{3, 2, 1});
    }

    SECTION("unique solo quita los repetidos ADYACENTES: hay que ordenar antes") {
        CHECK(ordenar_sin_repetidos({1, 2, 1, 2, 1}) == Enteros{2, 1});
    }
}

TEST_CASE("indices_negativos genera los indices con iota", "[rangos][iota]") {
    CHECK(indices_negativos({1, -2, 3, -4}) == Indices{1, 3});
    CHECK(indices_negativos({-1, -2}) == Indices{0, 1});
    CHECK(indices_negativos({1, 2, 3}) == Indices{});
    CHECK(indices_negativos({}) == Indices{});

    SECTION("el cero no es negativo") {
        CHECK(indices_negativos({0, -1, 0}) == Indices{1});
    }
}

TEST_CASE("unir intercala el separador", "[rangos][unir]") {
    CHECK(unir({"a", "b", "c"}, ", ") == "a, b, c");
    CHECK(unir({"solo"}, ", ") == "solo");
    CHECK(unir({}, ", ") == "");

    SECTION("el separador no queda colgando al final") {
        CHECK(unir({"x", "y"}, "-") == "x-y");
    }

    SECTION("separador vacio") {
        CHECK(unir({"a", "b"}, "") == "ab");
    }

    SECTION("palabras vacias") {
        CHECK(unir({"", "", ""}, ",") == ",,");
    }
}

TEST_CASE("trocear parte el texto por el separador", "[rangos][split]") {
    CHECK(trocear("a,b,c", ',') == Textos{"a", "b", "c"});
    CHECK(trocear("sin separadores", ',') == Textos{"sin separadores"});

    SECTION("los trozos vacios se conservan") {
        CHECK(trocear("a,,b", ',') == Textos{"a", "", "b"});
        CHECK(trocear(",", ',') == Textos{"", ""});
        CHECK(trocear("a,", ',') == Textos{"a", ""});
        CHECK(trocear(",a", ',') == Textos{"", "a"});
    }

    SECTION("un texto vacio no tiene trozos") {
        CHECK(trocear("", ',') == Textos{});
    }

    SECTION("el resultado sobrevive al texto de origen") {
        Textos trozos;
        {
            const std::string original = "uno dos tres";
            trozos = trocear(original, ' ');
        }
        // Si los trozos fueran vistas sobre `original`, esto seria un acceso a
        // memoria liberada.
        CHECK(trozos == Textos{"uno", "dos", "tres"});
    }
}

TEST_CASE("suma_cuadrados_en_rango combina filtro y ensanchado", "[rangos][suma]") {
    CHECK(suma_cuadrados_en_rango({1, 2, 3, 4, 5}, 2, 4) == 29);  // 4 + 9 + 16
    CHECK(suma_cuadrados_en_rango({1, 2, 3}, 10, 20) == 0);
    CHECK(suma_cuadrados_en_rango({}, 0, 10) == 0);

    SECTION("los limites son inclusivos") {
        CHECK(suma_cuadrados_en_rango({1, 2, 3}, 1, 3) == 14);
        CHECK(suma_cuadrados_en_rango({5}, 5, 5) == 25);
    }

    SECTION("con negativos") {
        CHECK(suma_cuadrados_en_rango({-3, -1, 2}, -2, 2) == 5);  // 1 + 4
    }

    SECTION("el cuadrado no puede desbordar un int") {
        // 50000 * 50000 = 2.5e9, por encima de INT_MAX.
        CHECK(suma_cuadrados_en_rango({50'000}, 0, 100'000) == 2'500'000'000LL);
        CHECK(suma_cuadrados_en_rango({50'000, 50'000}, 0, 100'000) == 5'000'000'000LL);
    }
}

TEST_CASE("las vistas son perezosas y no copian", "[rangos][pereza]") {
    // Este test no usa la interfaz de la leccion: comprueba la propiedad que
    // hace interesantes a las vistas.
    int evaluaciones = 0;
    const Enteros valores{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto vista = valores | std::views::filter([&evaluaciones](int valor) {
                     ++evaluaciones;
                     return valor % 2 == 0;
                 });

    // Construir la tuberia no ha ejecutado el filtro ni una sola vez...
    CHECK(evaluaciones == 0);

    // ...salvo lo justo para encontrar el primer elemento cuando se pide.
    const auto primero = *vista.begin();
    CHECK(primero == 2);
    CHECK(evaluaciones <= 2);

    SECTION("recorrer del todo evalua los que faltaban") {
        const int antes = evaluaciones;
        const auto cuantos = std::ranges::distance(vista);
        CHECK(cuantos == 5);
        CHECK(evaluaciones > antes);
    }
}
