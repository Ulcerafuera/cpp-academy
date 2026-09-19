#include "academy/errores.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <limits>
#include <optional>
#include <string>
#include <vector>

using academy::errores::a_entero;
using academy::errores::describir;
using academy::errores::dividir;
using academy::errores::Error;
using academy::errores::es_valor;
using academy::errores::evaluar;
using academy::errores::media;
using academy::errores::primero_mayor_que;
using academy::errores::Recuento;
using academy::errores::Resultado;
using academy::errores::resumir;
using academy::errores::valor_o;

using Catch::Matchers::WithinRel;

TEST_CASE("a_entero analiza el texto entero o nada", "[errores][optional][parsear]") {
    SECTION("numeros validos") {
        CHECK(a_entero("42") == 42);
        CHECK(a_entero("0") == 0);
        CHECK(a_entero("-7") == -7);
        CHECK(a_entero("2147483647") == 2147483647);
    }

    SECTION("lo que no es un numero") {
        CHECK(a_entero("") == std::nullopt);
        CHECK(a_entero("abc") == std::nullopt);
        CHECK(a_entero("3.5") == std::nullopt);
        CHECK(a_entero("--1") == std::nullopt);
    }

    SECTION("no vale con que EMPIECE por un numero") {
        // Si solo se comprueba el codigo de error y no que se haya consumido
        // todo el texto, estos cuatro se cuelan.
        CHECK(a_entero("42abc") == std::nullopt);
        CHECK(a_entero("42 ") == std::nullopt);
        CHECK(a_entero("1,5") == std::nullopt);
        CHECK(a_entero("7;") == std::nullopt);
    }

    SECTION("espacios y signos que from_chars no acepta") {
        CHECK(a_entero(" 42") == std::nullopt);
        CHECK(a_entero("+42") == std::nullopt);
    }

    SECTION("numeros que no caben en un int") {
        CHECK(a_entero("99999999999999999999") == std::nullopt);
        CHECK(a_entero("2147483648") == std::nullopt);
    }
}

TEST_CASE("media devuelve nullopt para una lista vacia", "[errores][optional]") {
    SECTION("casos normales") {
        CHECK(media({1, 2, 3}).value() == 2.0);
        CHECK(media({10}).value() == 10.0);
        CHECK_THAT(media({1, 2}).value(), WithinRel(1.5, 1e-12));
    }

    SECTION("la media no es una division entera") {
        // Con enteros, (1+2)/2 daria 1.
        CHECK_THAT(media({1, 2}).value(), WithinRel(1.5, 1e-12));
        CHECK_THAT(media({1, 1, 1, 2}).value(), WithinRel(1.25, 1e-12));
    }

    SECTION("la suma no desborda") {
        const std::vector<int> grandes(4, std::numeric_limits<int>::max());
        CHECK_THAT(media(grandes).value(),
                   WithinRel(static_cast<double>(std::numeric_limits<int>::max()), 1e-12));
    }

    SECTION("lista vacia") {
        CHECK(media({}) == std::nullopt);
        CHECK_FALSE(media({}).has_value());
        CHECK(media({}).value_or(-1.0) == -1.0);
    }
}

TEST_CASE("primero_mayor_que busca sin obligar a un centinela", "[errores][optional]") {
    const std::vector<int> valores{1, 5, 3, 9, 2};

    CHECK(primero_mayor_que(valores, 4) == 5);
    CHECK(primero_mayor_que(valores, 0) == 1);
    CHECK(primero_mayor_que(valores, 8) == 9);
    CHECK(primero_mayor_que(valores, 100) == std::nullopt);
    CHECK(primero_mayor_que({}, 0) == std::nullopt);

    SECTION("es ESTRICTAMENTE mayor") {
        CHECK(primero_mayor_que({5}, 5) == std::nullopt);
        CHECK(primero_mayor_que({5}, 4) == 5);
    }

    SECTION("con negativos, un centinela como -1 seria ambiguo") {
        CHECK(primero_mayor_que({-5, -1, -3}, -2) == -1);
    }
}

TEST_CASE("dividir devuelve un error en vez de infinito", "[errores][variant]") {
    SECTION("division normal") {
        const Resultado resultado = dividir(6.0, 3.0);
        REQUIRE(es_valor(resultado));
        CHECK_THAT(valor_o(resultado, -1.0), WithinRel(2.0, 1e-12));
    }

    SECTION("division por cero") {
        const Resultado resultado = dividir(1.0, 0.0);
        CHECK_FALSE(es_valor(resultado));
        CHECK(valor_o(resultado, -1.0) == -1.0);
        CHECK(describir(resultado) == "error: division por cero");
    }

    SECTION("cero entre algo si vale") {
        const Resultado resultado = dividir(0.0, 5.0);
        CHECK(es_valor(resultado));
        CHECK(valor_o(resultado, -1.0) == 0.0);
    }
}

TEST_CASE("describir cubre las dos alternativas", "[errores][variant][visit]") {
    CHECK(describir(dividir(6.0, 3.0)) == "2");
    CHECK(describir(dividir(5.0, 2.0)) == "2.5");
    CHECK(describir(dividir(-9.0, 3.0)) == "-3");
    CHECK(describir(dividir(1.0, 0.0)) == "error: division por cero");
    CHECK(describir(Resultado{Error{"vaya"}}) == "error: vaya");
}

TEST_CASE("evaluar analiza una expresion completa", "[errores][variant][evaluar]") {
    SECTION("las cuatro operaciones") {
        CHECK_THAT(valor_o(evaluar("2 + 2"), -1.0), WithinRel(4.0, 1e-12));
        CHECK_THAT(valor_o(evaluar("10 - 4"), -1.0), WithinRel(6.0, 1e-12));
        CHECK_THAT(valor_o(evaluar("3 * 7"), -1.0), WithinRel(21.0, 1e-12));
        CHECK_THAT(valor_o(evaluar("6 / 3"), -1.0), WithinRel(2.0, 1e-12));
    }

    SECTION("numeros con decimales y negativos") {
        CHECK_THAT(valor_o(evaluar("2.5 * 4"), -1.0), WithinRel(10.0, 1e-12));
        CHECK_THAT(valor_o(evaluar("-3 + 1"), -1.0), WithinRel(-2.0, 1e-12));
    }

    SECTION("la division por cero se propaga como error") {
        const Resultado resultado = evaluar("1 / 0");
        CHECK_FALSE(es_valor(resultado));
        CHECK(describir(resultado) == "error: division por cero");
    }

    SECTION("formato invalido") {
        const std::string esperado =
            "error: formato invalido: se esperaba '<numero> <operador> <numero>'";
        CHECK(describir(evaluar("hola")) == esperado);
        CHECK(describir(evaluar("")) == esperado);
        CHECK(describir(evaluar("2 +")) == esperado);
    }

    SECTION("operandos que no son numeros") {
        CHECK(describir(evaluar("a + 2")) == "error: no es un numero: a");
        CHECK(describir(evaluar("2 + b")) == "error: no es un numero: b");
    }

    SECTION("operador desconocido") {
        CHECK(describir(evaluar("2 ^ 3")) == "error: operador desconocido: ^");
        CHECK(describir(evaluar("2 ++ 3")) == "error: operador desconocido: ++");
    }
}

TEST_CASE("resumir separa los aciertos de los fallos", "[errores][variant][resumir]") {
    SECTION("mezcla de valores y errores") {
        const std::vector<Resultado> resultados{
            dividir(10.0, 2.0),  // 5
            dividir(1.0, 0.0),   // error
            dividir(9.0, 3.0),   // 3
            evaluar("mal"),      // error
        };

        const Recuento recuento = resumir(resultados);
        CHECK_THAT(recuento.suma, WithinRel(8.0, 1e-12));
        CHECK(recuento.errores == 2U);
    }

    SECTION("todo correcto") {
        const std::vector<Resultado> resultados{dividir(4.0, 2.0), dividir(6.0, 2.0)};
        CHECK(resumir(resultados) == Recuento{5.0, 0});
    }

    SECTION("todo mal") {
        const std::vector<Resultado> resultados{dividir(1.0, 0.0), dividir(2.0, 0.0)};
        CHECK(resumir(resultados) == Recuento{0.0, 2});
    }

    SECTION("lista vacia") {
        CHECK(resumir({}) == Recuento{});
    }
}
