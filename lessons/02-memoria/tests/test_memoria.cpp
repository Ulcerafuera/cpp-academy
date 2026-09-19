#include "academy/memoria.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <limits>
#include <stdexcept>
#include <vector>

using academy::memoria::invertir;
using academy::memoria::liberar_secuencia;
using academy::memoria::reservar_secuencia;
using academy::memoria::suma;

namespace {

/// Guarda un bloque de reservar_secuencia() para que los tests no filtren
/// memoria aunque una comprobacion falle a mitad. Es RAII en miniatura: justo
/// lo que construiras a mano en la leccion 03.
class BloqueReservado {
public:
    explicit BloqueReservado(std::size_t n) : datos_(reservar_secuencia(n)), tam_(n) {}
    ~BloqueReservado() { liberar_secuencia(datos_); }

    BloqueReservado(const BloqueReservado&) = delete;
    BloqueReservado& operator=(const BloqueReservado&) = delete;

    int* datos() const noexcept { return datos_; }
    std::size_t tam() const noexcept { return tam_; }

private:
    int* datos_;
    std::size_t tam_;
};

}  // namespace

TEST_CASE("suma recorre un bloque de enteros", "[memoria][suma]") {
    SECTION("bloque normal") {
        const std::array<int, 5> valores{1, 2, 3, 4, 5};
        CHECK(suma(valores.data(), valores.size()) == 15);
    }

    SECTION("solo una parte del bloque") {
        const std::array<int, 5> valores{1, 2, 3, 4, 5};
        CHECK(suma(valores.data(), 3) == 6);
        CHECK(suma(valores.data(), 1) == 1);
    }

    SECTION("bloque vacio") {
        const std::array<int, 3> valores{7, 8, 9};
        CHECK(suma(valores.data(), 0) == 0);
        CHECK(suma(nullptr, 0) == 0);
    }

    SECTION("valores negativos") {
        const std::array<int, 4> valores{-5, 5, -10, 10};
        CHECK(suma(valores.data(), valores.size()) == 0);
    }

    SECTION("el acumulador no desborda un int") {
        const std::vector<int> valores(4, std::numeric_limits<int>::max());
        CHECK(suma(valores.data(), valores.size()) == 4LL * std::numeric_limits<int>::max());
    }
}

TEST_CASE("suma rechaza un puntero nulo con tamano positivo", "[memoria][suma][errores]") {
    CHECK_THROWS_AS(suma(nullptr, 1), std::invalid_argument);
    CHECK_THROWS_AS(suma(nullptr, 100), std::invalid_argument);
}

TEST_CASE("invertir da la vuelta al bloque in situ", "[memoria][invertir]") {
    SECTION("numero par de elementos") {
        std::array<int, 4> valores{1, 2, 3, 4};
        invertir(valores.data(), valores.size());
        CHECK(valores == std::array<int, 4>{4, 3, 2, 1});
    }

    SECTION("numero impar de elementos: el central se queda donde esta") {
        std::array<int, 5> valores{1, 2, 3, 4, 5};
        invertir(valores.data(), valores.size());
        CHECK(valores == std::array<int, 5>{5, 4, 3, 2, 1});
    }

    SECTION("un solo elemento") {
        std::array<int, 1> valores{42};
        invertir(valores.data(), valores.size());
        CHECK(valores == std::array<int, 1>{42});
    }

    SECTION("bloque vacio: no debe tocar nada ni desbordar el indice") {
        std::array<int, 2> valores{1, 2};
        invertir(valores.data(), 0);
        CHECK(valores == std::array<int, 2>{1, 2});
        CHECK_NOTHROW(invertir(nullptr, 0));
    }

    SECTION("invertir dos veces devuelve el original") {
        std::array<int, 6> valores{3, 1, 4, 1, 5, 9};
        const auto original = valores;
        invertir(valores.data(), valores.size());
        invertir(valores.data(), valores.size());
        CHECK(valores == original);
    }
}

TEST_CASE("invertir rechaza un puntero nulo con tamano positivo", "[memoria][invertir][errores]") {
    CHECK_THROWS_AS(invertir(nullptr, 1), std::invalid_argument);
}

TEST_CASE("reservar_secuencia entrega un bloque inicializado", "[memoria][reservar]") {
    SECTION("los valores van de 0 a n-1") {
        const BloqueReservado bloque(10);
        REQUIRE(bloque.datos() != nullptr);
        for (std::size_t i = 0; i < bloque.tam(); ++i) {
            CAPTURE(i);
            CHECK(bloque.datos()[i] == static_cast<int>(i));
        }
    }

    SECTION("el bloque es escribible en todo su rango") {
        const BloqueReservado bloque(64);
        REQUIRE(bloque.datos() != nullptr);
        for (std::size_t i = 0; i < bloque.tam(); ++i) {
            bloque.datos()[i] = -1;
        }
        CHECK(suma(bloque.datos(), bloque.tam()) == -64);
    }

    SECTION("n == 0 devuelve nullptr") {
        const BloqueReservado bloque(0);
        CHECK(bloque.datos() == nullptr);
    }

    SECTION("liberar_secuencia acepta nullptr") {
        CHECK_NOTHROW(liberar_secuencia(nullptr));
    }
}

TEST_CASE("las funciones de la leccion encajan entre si", "[memoria][integracion]") {
    const BloqueReservado bloque(100);
    REQUIRE(bloque.datos() != nullptr);

    // 0 + 1 + ... + 99
    CHECK(suma(bloque.datos(), bloque.tam()) == 4950);

    invertir(bloque.datos(), bloque.tam());
    CHECK(bloque.datos()[0] == 99);
    CHECK(bloque.datos()[99] == 0);
    CHECK(suma(bloque.datos(), bloque.tam()) == 4950);
}
