#include "academy/texto.hpp"

#include <catch2/catch_test_macros.hpp>

#include <map>
#include <string>
#include <utility>
#include <vector>

using academy::stl::contar_palabras;
using academy::stl::top_n;

using Frecuencias = std::map<std::string, int>;
using Ranking = std::vector<std::pair<std::string, int>>;

TEST_CASE("contar_palabras separa el texto en palabras", "[stl][contar]") {
    SECTION("texto sencillo") {
        const Frecuencias esperado{{"el", 1}, {"gato", 2}, {"come", 1}};
        CHECK(contar_palabras("el gato come gato") == esperado);
    }

    SECTION("texto vacio o solo separadores") {
        CHECK(contar_palabras("").empty());
        CHECK(contar_palabras("   ").empty());
        CHECK(contar_palabras(" ... !!! \n\t ").empty());
    }

    SECTION("una sola palabra, sin separador final") {
        const Frecuencias esperado{{"hola", 1}};
        CHECK(contar_palabras("hola") == esperado);
    }

    SECTION("los separadores consecutivos no generan palabras vacias") {
        const Frecuencias esperado{{"uno", 1}, {"dos", 1}};
        CHECK(contar_palabras("  uno,,,   dos  ") == esperado);
    }
}

TEST_CASE("contar_palabras normaliza a minusculas", "[stl][contar][minusculas]") {
    const Frecuencias esperado{{"casa", 3}};
    CHECK(contar_palabras("Casa casa CASA") == esperado);
    CHECK(contar_palabras("CaSa cAsA CASa") == esperado);
}

TEST_CASE("contar_palabras trata la puntuacion como separador", "[stl][contar][puntuacion]") {
    SECTION("signos habituales") {
        const Frecuencias esperado{{"hola", 1}, {"que", 1}, {"tal", 1}};
        CHECK(contar_palabras("Hola, que tal!") == esperado);
    }

    SECTION("saltos de linea y tabuladores") {
        const Frecuencias esperado{{"a", 1}, {"b", 1}, {"c", 1}};
        CHECK(contar_palabras("a\nb\tc") == esperado);
    }

    SECTION("los digitos SI forman parte de las palabras") {
        const Frecuencias esperado{{"c17", 1}, {"cpp20", 1}, {"2026", 1}};
        CHECK(contar_palabras("c17 cpp20 2026") == esperado);
    }

    SECTION("el guion parte la palabra en dos") {
        const Frecuencias esperado{{"bien", 1}, {"hecho", 1}};
        CHECK(contar_palabras("bien-hecho") == esperado);
    }
}

TEST_CASE("contar_palabras no se rompe con bytes no ASCII", "[stl][contar][utf8]") {
    // Los bytes de UTF-8 actuan como separadores: "accion" con tilde se parte
    // en "acci" y "n". Es la limitacion documentada; lo importante es que no
    // haya comportamiento indefinido al pasarlos a std::isalnum.
    // Los literales se parten porque un escape hexadecimal consume TODOS los
    // digitos hex que le siguen: "\xb1ana" seria un solo escape enorme.
    const auto frecuencias = contar_palabras(
        "acci\xc3\xb3n ma\xc3\xb1"
        "ana");
    CHECK(frecuencias.at("acci") == 1);
    CHECK(frecuencias.at("n") == 1);
    CHECK(frecuencias.at("ma") == 1);
    CHECK(frecuencias.at("ana") == 1);
}

TEST_CASE("top_n ordena por frecuencia descendente", "[stl][top_n]") {
    const Frecuencias frecuencias{{"a", 5}, {"b", 3}, {"c", 9}, {"d", 1}};

    SECTION("las dos mas frecuentes") {
        const Ranking esperado{{"c", 9}, {"a", 5}};
        CHECK(top_n(frecuencias, 2) == esperado);
    }

    SECTION("todas") {
        const Ranking esperado{{"c", 9}, {"a", 5}, {"b", 3}, {"d", 1}};
        CHECK(top_n(frecuencias, 4) == esperado);
    }

    SECTION("pedir mas de las que hay devuelve todas") {
        CHECK(top_n(frecuencias, 100).size() == 4);
        CHECK(top_n(frecuencias, 100).front() == std::pair<std::string, int>{"c", 9});
    }

    SECTION("pedir cero devuelve un ranking vacio") {
        CHECK(top_n(frecuencias, 0).empty());
    }

    SECTION("un mapa vacio devuelve un ranking vacio") {
        CHECK(top_n(Frecuencias{}, 5).empty());
    }
}

TEST_CASE("top_n desempata por orden alfabetico", "[stl][top_n][desempate]") {
    const Frecuencias frecuencias{{"pera", 2}, {"kiwi", 2}, {"uva", 2}, {"mango", 7}};

    const Ranking esperado{{"mango", 7}, {"kiwi", 2}, {"pera", 2}, {"uva", 2}};
    CHECK(top_n(frecuencias, 4) == esperado);

    SECTION("el desempate tambien se aplica al recortar") {
        const Ranking dos{{"mango", 7}, {"kiwi", 2}};
        CHECK(top_n(frecuencias, 2) == dos);
    }
}

TEST_CASE("contar_palabras y top_n encajan entre si", "[stl][integracion]") {
    constexpr std::string_view texto =
        "El perro y el gato. El perro ladra; el gato maulla. "
        "Y el perro corre.";

    const auto frecuencias = contar_palabras(texto);
    CHECK(frecuencias.at("el") == 5);
    CHECK(frecuencias.at("perro") == 3);
    CHECK(frecuencias.at("gato") == 2);
    CHECK(frecuencias.at("y") == 2);
    CHECK(frecuencias.at("ladra") == 1);

    const Ranking esperado{{"el", 5}, {"perro", 3}, {"gato", 2}};
    CHECK(top_n(frecuencias, 3) == esperado);
}
