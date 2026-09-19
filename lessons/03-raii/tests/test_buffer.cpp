#include "academy/buffer.hpp"

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

using academy::raii::Buffer;

namespace {

/// Crea un Buffer de `n` elementos con los valores 0, 1, ..., n-1.
Buffer con_secuencia(std::size_t n) {
    Buffer buffer(n);
    for (std::size_t i = 0; i < n; ++i) {
        buffer[i] = static_cast<int>(i);
    }
    return buffer;
}

}  // namespace

TEST_CASE("un Buffer recien construido cumple su invariante", "[raii][construccion]") {
    SECTION("por defecto esta vacio y no reserva nada") {
        const Buffer buffer;
        CHECK(buffer.tam() == 0);
        CHECK(buffer.vacio());
        CHECK(buffer.datos() == nullptr);
    }

    SECTION("con tamano n reserva n enteros a cero") {
        const Buffer buffer(5);
        REQUIRE(buffer.tam() == 5);
        CHECK_FALSE(buffer.vacio());
        REQUIRE(buffer.datos() != nullptr);
        for (std::size_t i = 0; i < buffer.tam(); ++i) {
            CAPTURE(i);
            CHECK(buffer[i] == 0);
        }
    }

    SECTION("con tamano 0 no reserva") {
        const Buffer buffer(0);
        CHECK(buffer.tam() == 0);
        CHECK(buffer.datos() == nullptr);
    }
}

TEST_CASE("en() comprueba el indice y operator[] no", "[raii][acceso]") {
    Buffer buffer = con_secuencia(3);

    CHECK(buffer.en(0) == 0);
    CHECK(buffer.en(2) == 2);

    buffer.en(1) = 99;
    CHECK(buffer[1] == 99);

    CHECK_THROWS_AS(buffer.en(3), std::out_of_range);
    CHECK_THROWS_AS(buffer.en(100), std::out_of_range);

    const Buffer vacio;
    CHECK_THROWS_AS(vacio.en(0), std::out_of_range);
}

TEST_CASE("la copia es profunda", "[raii][copia]") {
    SECTION("constructor de copia") {
        const Buffer original = con_secuencia(4);
        Buffer copia(original);

        REQUIRE(copia.tam() == original.tam());
        CHECK(copia == original);
        // Dos bloques distintos: si el puntero se compartiera, el segundo
        // destructor liberaria memoria ya liberada.
        CHECK(copia.datos() != original.datos());

        copia[0] = 1000;
        CHECK(original[0] == 0);
    }

    SECTION("asignacion por copia libera lo que hubiera antes") {
        const Buffer original = con_secuencia(4);
        Buffer destino = con_secuencia(10);

        destino = original;

        REQUIRE(destino.tam() == 4);
        CHECK(destino == original);
        CHECK(destino.datos() != original.datos());
    }

    SECTION("asignar un buffer vacio deja el destino vacio") {
        Buffer destino = con_secuencia(6);
        const Buffer vacio;

        destino = vacio;

        CHECK(destino.tam() == 0);
        CHECK(destino.datos() == nullptr);
    }

    SECTION("copiar un buffer vacio") {
        const Buffer vacio;
        const Buffer copia(vacio);
        CHECK(copia.tam() == 0);
        CHECK(copia.datos() == nullptr);
    }
}

TEST_CASE("la autoasignacion no destruye el contenido", "[raii][copia][autoasignacion]") {
    Buffer buffer = con_secuencia(5);
    const Buffer esperado = con_secuencia(5);

    // La referencia intermedia evita que el compilador avise de la
    // autoasignacion evidente; el caso hay que soportarlo igualmente.
    Buffer& alias = buffer;
    buffer = alias;

    REQUIRE(buffer.tam() == 5);
    CHECK(buffer == esperado);
}

TEST_CASE("el movimiento roba el bloque sin copiarlo", "[raii][movimiento]") {
    SECTION("constructor de movimiento") {
        Buffer origen = con_secuencia(4);
        const int* bloque = origen.datos();

        Buffer destino(std::move(origen));

        REQUIRE(destino.tam() == 4);
        CHECK(destino.datos() == bloque);  // mismo bloque: no se ha copiado
        CHECK(destino == con_secuencia(4));

        // El origen queda vacio pero valido: se le puede seguir llamando.
        CHECK(origen.tam() == 0);  // NOLINT(bugprone-use-after-move)
        CHECK(origen.datos() == nullptr);
        CHECK(origen.vacio());
    }

    SECTION("asignacion por movimiento") {
        Buffer origen = con_secuencia(4);
        const int* bloque = origen.datos();
        Buffer destino = con_secuencia(10);

        destino = std::move(origen);

        REQUIRE(destino.tam() == 4);
        CHECK(destino.datos() == bloque);
        CHECK(origen.tam() == 0);  // NOLINT(bugprone-use-after-move)
        CHECK(origen.datos() == nullptr);
    }

    SECTION("un buffer movido se puede reutilizar") {
        Buffer origen = con_secuencia(4);
        Buffer destino(std::move(origen));

        origen = con_secuencia(2);  // NOLINT(bugprone-use-after-move)

        REQUIRE(origen.tam() == 2);
        CHECK(origen[0] == 0);
        CHECK(origen[1] == 1);
        CHECK(destino.tam() == 4);
    }

    SECTION("la automovida no deja el objeto inservible") {
        Buffer buffer = con_secuencia(5);
        Buffer& alias = buffer;
        buffer = std::move(alias);

        // El estandar solo exige que quede valido; aqui se comprueba que
        // ademas no se haya liberado el bloque por debajo.
        CHECK(buffer.tam() == 5);
    }
}

TEST_CASE("intercambiar no copia ni reserva", "[raii][intercambiar]") {
    Buffer a = con_secuencia(3);
    Buffer b = con_secuencia(7);
    const int* bloque_a = a.datos();
    const int* bloque_b = b.datos();

    a.intercambiar(b);

    CHECK(a.tam() == 7);
    CHECK(b.tam() == 3);
    CHECK(a.datos() == bloque_b);
    CHECK(b.datos() == bloque_a);

    SECTION("intercambiar con un buffer vacio") {
        Buffer vacio;
        a.intercambiar(vacio);
        CHECK(a.tam() == 0);
        CHECK(a.datos() == nullptr);
        CHECK(vacio.tam() == 7);
    }
}

TEST_CASE("Buffer se comporta como un tipo de valor", "[raii][semantica]") {
    STATIC_REQUIRE(std::is_copy_constructible_v<Buffer>);
    STATIC_REQUIRE(std::is_copy_assignable_v<Buffer>);
    STATIC_REQUIRE(std::is_move_constructible_v<Buffer>);
    STATIC_REQUIRE(std::is_move_assignable_v<Buffer>);
    STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Buffer>);
    STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Buffer>);
    STATIC_REQUIRE(std::is_nothrow_destructible_v<Buffer>);

    SECTION("por eso se puede guardar en un contenedor de la STL") {
        std::vector<Buffer> buffers;
        buffers.reserve(2);
        buffers.push_back(con_secuencia(3));
        buffers.push_back(con_secuencia(5));
        buffers.push_back(con_secuencia(7));  // fuerza una reubicacion

        REQUIRE(buffers.size() == 3);
        CHECK(buffers[0] == con_secuencia(3));
        CHECK(buffers[1] == con_secuencia(5));
        CHECK(buffers[2] == con_secuencia(7));
    }
}
