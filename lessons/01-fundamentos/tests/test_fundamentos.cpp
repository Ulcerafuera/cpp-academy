#include "academy/fundamentos.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <stdexcept>
#include <string>
#include <vector>

using academy::fundamentos::es_primo;
using academy::fundamentos::fizzbuzz;
using academy::fundamentos::mcd;

TEST_CASE("fizzbuzz sustituye los multiplos de 3 y de 5", "[fundamentos][fizzbuzz]") {
    SECTION("numeros que no son multiplos") {
        CHECK(fizzbuzz(1) == "1");
        CHECK(fizzbuzz(2) == "2");
        CHECK(fizzbuzz(7) == "7");
        CHECK(fizzbuzz(101) == "101");
    }

    SECTION("multiplos de 3") {
        CHECK(fizzbuzz(3) == "Fizz");
        CHECK(fizzbuzz(9) == "Fizz");
        CHECK(fizzbuzz(33) == "Fizz");
    }

    SECTION("multiplos de 5") {
        CHECK(fizzbuzz(5) == "Buzz");
        CHECK(fizzbuzz(20) == "Buzz");
        CHECK(fizzbuzz(100) == "Buzz");
    }

    SECTION("multiplos de 3 y de 5 a la vez") {
        CHECK(fizzbuzz(15) == "FizzBuzz");
        CHECK(fizzbuzz(30) == "FizzBuzz");
        CHECK(fizzbuzz(45) == "FizzBuzz");
    }

    SECTION("la secuencia completa del 1 al 15") {
        const std::vector<std::string> esperado{"1",    "2",    "Fizz", "4",    "Buzz",
                                                "Fizz", "7",    "8",    "Fizz", "Buzz",
                                                "11",   "Fizz", "13",   "14",   "FizzBuzz"};
        std::vector<std::string> obtenido;
        obtenido.reserve(esperado.size());
        for (int n = 1; n <= 15; ++n) {
            obtenido.push_back(fizzbuzz(n));
        }
        CHECK(obtenido == esperado);
    }
}

TEST_CASE("fizzbuzz rechaza los valores fuera de rango", "[fundamentos][fizzbuzz][errores]") {
    CHECK_THROWS_AS(fizzbuzz(0), std::invalid_argument);
    CHECK_THROWS_AS(fizzbuzz(-1), std::invalid_argument);
    CHECK_THROWS_AS(fizzbuzz(-15), std::invalid_argument);
}

TEST_CASE("mcd calcula el maximo comun divisor", "[fundamentos][mcd]") {
    SECTION("casos habituales") {
        CHECK(mcd(12, 18) == 6);
        CHECK(mcd(18, 12) == 6);
        CHECK(mcd(17, 5) == 1);
        CHECK(mcd(270, 192) == 6);
        CHECK(mcd(1071, 462) == 21);
    }

    SECTION("el cero es el elemento neutro") {
        CHECK(mcd(0, 9) == 9);
        CHECK(mcd(9, 0) == 9);
        CHECK(mcd(0, 0) == 0);
    }

    SECTION("el resultado nunca es negativo") {
        CHECK(mcd(-12, 18) == 6);
        CHECK(mcd(12, -18) == 6);
        CHECK(mcd(-12, -18) == 6);
    }

    SECTION("un numero es divisor del otro") {
        CHECK(mcd(5, 25) == 5);
        CHECK(mcd(100, 10) == 10);
    }
}

TEST_CASE("es_primo reconoce los numeros primos", "[fundamentos][primos]") {
    SECTION("primos pequenos") {
        const int n = GENERATE(2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 97);
        CAPTURE(n);
        CHECK(es_primo(n));
    }

    SECTION("compuestos") {
        const int n = GENERATE(4, 6, 8, 9, 15, 21, 25, 27, 49, 91, 100);
        CAPTURE(n);
        CHECK_FALSE(es_primo(n));
    }

    SECTION("los valores menores que 2 no son primos") {
        const int n = GENERATE(1, 0, -1, -7);
        CAPTURE(n);
        CHECK_FALSE(es_primo(n));
    }

    SECTION("primos grandes sin desbordar") {
        CHECK(es_primo(2147483647));  // 2^31 - 1, primo de Mersenne
        CHECK_FALSE(es_primo(2147483646));
    }
}
