#include "academy/cola_segura.hpp"
#include "academy/paralelo.hpp"

#include <catch2/catch_test_macros.hpp>

#include <atomic>
#include <chrono>
#include <future>
#include <numeric>
#include <set>
#include <stdexcept>
#include <thread>
#include <vector>

using academy::concurrencia::ColaCerrada;
using academy::concurrencia::ColaSegura;
using academy::concurrencia::hilos_disponibles;
using academy::concurrencia::suma_paralela;

namespace {

/// Espera a un future con limite de tiempo. Si una implementacion se queda
/// bloqueada, el test falla en unos segundos en vez de colgar la suite entera.
template<typename T>
bool listo_antes_de(std::future<T>& futuro, std::chrono::milliseconds limite) {
    return futuro.wait_for(limite) == std::future_status::ready;
}

constexpr auto kLimite = std::chrono::seconds(5);

}  // namespace

TEST_CASE("la cola funciona con un solo hilo", "[concurrencia][cola]") {
    ColaSegura<int> cola;

    SECTION("empieza vacia y abierta") {
        CHECK(cola.vacia());
        CHECK(cola.tam() == 0);
        CHECK_FALSE(cola.cerrada());
    }

    SECTION("respeta el orden FIFO") {
        cola.poner(1);
        cola.poner(2);
        cola.poner(3);

        REQUIRE(cola.tam() == 3);
        CHECK(cola.tomar() == 1);
        CHECK(cola.tomar() == 2);
        CHECK(cola.tomar() == 3);
        CHECK(cola.vacia());
    }

    SECTION("intentar_tomar no bloquea cuando no hay nada") {
        CHECK_FALSE(cola.intentar_tomar().has_value());

        cola.poner(42);
        const auto valor = cola.intentar_tomar();
        REQUIRE(valor.has_value());
        CHECK(*valor == 42);
        CHECK_FALSE(cola.intentar_tomar().has_value());
    }
}

TEST_CASE("cerrar la cola despierta a los consumidores", "[concurrencia][cola][cierre]") {
    SECTION("tomar de una cola cerrada y vacia es un error") {
        ColaSegura<int> cola;
        cola.cerrar();
        CHECK(cola.cerrada());
        CHECK_THROWS_AS(cola.tomar(), ColaCerrada);
    }

    SECTION("lo que quedo encolado antes del cierre se puede seguir tomando") {
        ColaSegura<int> cola;
        cola.poner(1);
        cola.poner(2);
        cola.cerrar();

        CHECK(cola.tomar() == 1);
        CHECK(cola.tomar() == 2);
        CHECK_THROWS_AS(cola.tomar(), ColaCerrada);
    }

    SECTION("poner en una cola cerrada es un error") {
        ColaSegura<int> cola;
        cola.cerrar();
        CHECK_THROWS_AS(cola.poner(1), ColaCerrada);
    }

    SECTION("cerrar dos veces no pasa nada") {
        ColaSegura<int> cola;
        cola.cerrar();
        CHECK_NOTHROW(cola.cerrar());
        CHECK(cola.cerrada());
    }

    SECTION("cerrar desbloquea a TODOS los consumidores que esperan") {
        ColaSegura<int> cola;
        constexpr int kConsumidores = 4;

        std::vector<std::future<bool>> esperando;
        esperando.reserve(kConsumidores);
        for (int i = 0; i < kConsumidores; ++i) {
            esperando.push_back(std::async(std::launch::async, [&cola] {
                try {
                    (void)cola.tomar();
                    return false;
                } catch (const ColaCerrada&) {
                    return true;  // desbloqueado por el cierre: lo esperado
                }
            }));
        }

        // Da tiempo a que los cuatro lleguen al wait antes de cerrar.
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        cola.cerrar();

        for (auto& futuro : esperando) {
            REQUIRE(listo_antes_de(futuro, kLimite));
            CHECK(futuro.get());
        }
    }
}

TEST_CASE("tomar espera a que llegue un elemento", "[concurrencia][cola][bloqueo]") {
    ColaSegura<int> cola;

    auto consumidor = std::async(std::launch::async, [&cola] { return cola.tomar(); });

    // Mientras no se ponga nada, el consumidor sigue esperando.
    CHECK(consumidor.wait_for(std::chrono::milliseconds(50)) == std::future_status::timeout);

    cola.poner(7);

    REQUIRE(listo_antes_de(consumidor, kLimite));
    CHECK(consumidor.get() == 7);
}

TEST_CASE("varios productores y consumidores a la vez", "[concurrencia][cola][estres]") {
    constexpr int kProductores = 4;
    constexpr int kConsumidores = 4;
    constexpr int kPorProductor = 250;
    constexpr int kTotal = kProductores * kPorProductor;

    ColaSegura<int> cola;

    std::vector<std::future<std::vector<int>>> consumidores;
    consumidores.reserve(kConsumidores);
    for (int i = 0; i < kConsumidores; ++i) {
        consumidores.push_back(std::async(std::launch::async, [&cola] {
            std::vector<int> recibidos;
            try {
                while (true) {
                    recibidos.push_back(cola.tomar());
                }
            } catch (const ColaCerrada&) {
                // Fin normal: la cola se cerro y se vacio.
            }
            return recibidos;
        }));
    }

    std::vector<std::future<void>> productores;
    productores.reserve(kProductores);
    for (int p = 0; p < kProductores; ++p) {
        productores.push_back(std::async(std::launch::async, [&cola, p] {
            for (int i = 0; i < kPorProductor; ++i) {
                cola.poner(p * kPorProductor + i);
            }
        }));
    }

    for (auto& productor : productores) {
        REQUIRE(listo_antes_de(productor, kLimite));
        productor.get();
    }
    cola.cerrar();

    std::multiset<int> todos;
    for (auto& consumidor : consumidores) {
        REQUIRE(listo_antes_de(consumidor, kLimite));
        for (const int valor : consumidor.get()) {
            todos.insert(valor);
        }
    }

    // Ni se pierde ni se duplica nada: cada valor aparece exactamente una vez.
    REQUIRE(todos.size() == static_cast<std::size_t>(kTotal));
    for (int valor = 0; valor < kTotal; ++valor) {
        REQUIRE(todos.count(valor) == 1U);
    }
}

TEST_CASE("suma_paralela da el mismo resultado que la secuencial", "[concurrencia][suma]") {
    SECTION("vector pequeno") {
        const std::vector<int> valores{1, 2, 3, 4, 5};
        for (unsigned hilos : {1U, 2U, 3U, 4U, 8U}) {
            CAPTURE(hilos);
            CHECK(suma_paralela(valores, hilos) == 15);
        }
    }

    SECTION("vector grande: ningun elemento se queda fuera ni se cuenta dos veces") {
        std::vector<int> valores(10'000);
        std::iota(valores.begin(), valores.end(), 1);
        const long long esperado = 10'000LL * 10'001LL / 2;

        for (unsigned hilos : {1U, 2U, 3U, 5U, 7U, 16U}) {
            CAPTURE(hilos);
            CHECK(suma_paralela(valores, hilos) == esperado);
        }
    }

    SECTION("el reparto desigual tampoco pierde elementos") {
        // 7 elementos entre 3 hilos: 3 + 2 + 2.
        const std::vector<int> valores{1, 1, 1, 1, 1, 1, 1};
        CHECK(suma_paralela(valores, 3) == 7);
        CHECK(suma_paralela(valores, 5) == 7);
    }

    SECTION("mas hilos que elementos") {
        const std::vector<int> valores{10, 20};
        CHECK(suma_paralela(valores, 100) == 30);
    }

    SECTION("vector vacio") {
        CHECK(suma_paralela(std::vector<int>{}, 4) == 0);
    }

    SECTION("valores negativos y acumulador ancho") {
        const std::vector<int> valores(1000, -2'000'000);
        CHECK(suma_paralela(valores, 4) == -2'000'000'000LL);
    }
}

TEST_CASE("suma_paralela valida sus argumentos", "[concurrencia][suma][errores]") {
    CHECK_THROWS_AS(suma_paralela(std::vector<int>{1, 2, 3}, 0), std::invalid_argument);
}

TEST_CASE("hilos_disponibles devuelve al menos 1", "[concurrencia][hilos]") {
    CHECK(hilos_disponibles() >= 1U);
}
