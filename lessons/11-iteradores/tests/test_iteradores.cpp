#include "academy/lista.hpp"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <iterator>
#include <numeric>
#include <ranges>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

using academy::iteradores::Lista;

namespace {

Lista<int> lista_de(std::initializer_list<int> valores) {
    Lista<int> lista;
    for (const int valor : valores) {
        lista.anadir(valor);
    }
    return lista;
}

std::vector<int> recorrer(const Lista<int>& lista) {
    std::vector<int> resultado;
    for (const int valor : lista) {
        resultado.push_back(valor);
    }
    return resultado;
}

}  // namespace

TEST_CASE("el iterador cumple el contrato de la biblioteca estandar", "[iteradores][concepto]") {
    // Si alguno de estos falla, ni el `for` de rango ni <algorithm> ni
    // <ranges> aceptaran la Lista.
    STATIC_REQUIRE(std::forward_iterator<Lista<int>::iterator>);
    STATIC_REQUIRE(std::forward_iterator<Lista<int>::const_iterator>);
    STATIC_REQUIRE(std::ranges::forward_range<Lista<int>>);
    STATIC_REQUIRE(std::ranges::forward_range<const Lista<int>>);

    SECTION("los alias son los que espera la biblioteca") {
        using It = Lista<int>::iterator;
        STATIC_REQUIRE(std::is_same_v<std::iter_value_t<It>, int>);
        STATIC_REQUIRE(std::is_same_v<std::iter_reference_t<It>, int&>);
        STATIC_REQUIRE(std::is_same_v<std::iter_difference_t<It>, std::ptrdiff_t>);

        using CIt = Lista<int>::const_iterator;
        STATIC_REQUIRE(std::is_same_v<std::iter_value_t<CIt>, int>);
        STATIC_REQUIRE(std::is_same_v<std::iter_reference_t<CIt>, const int&>);
    }

    SECTION("un iterador de avance se puede construir por defecto") {
        STATIC_REQUIRE(std::is_default_constructible_v<Lista<int>::iterator>);
        const Lista<int>::iterator a;
        const Lista<int>::iterator b;
        CHECK(a == b);
    }

    SECTION("la Lista es de solo-movimiento") {
        STATIC_REQUIRE_FALSE(std::is_copy_constructible_v<Lista<int>>);
        STATIC_REQUIRE_FALSE(std::is_copy_assignable_v<Lista<int>>);
        STATIC_REQUIRE(std::is_nothrow_move_constructible_v<Lista<int>>);
        STATIC_REQUIRE(std::is_nothrow_move_assignable_v<Lista<int>>);
    }
}

TEST_CASE("anadir encadena por el final", "[iteradores][anadir]") {
    SECTION("lista recien creada") {
        const Lista<int> lista;
        CHECK(lista.vacia());
        CHECK(lista.tam() == 0);
        CHECK(lista.begin() == lista.end());
    }

    SECTION("el orden es el de insercion") {
        const Lista<int> lista = lista_de({1, 2, 3, 4});
        CHECK(lista.tam() == 4);
        CHECK_FALSE(lista.vacia());
        CHECK(recorrer(lista) == std::vector<int>{1, 2, 3, 4});
    }

    SECTION("un solo elemento") {
        const Lista<int> lista = lista_de({42});
        CHECK(lista.tam() == 1);
        CHECK(recorrer(lista) == std::vector<int>{42});
        CHECK(lista.primero() == 42);
    }

    SECTION("muchos elementos, sin desbordar la pila al destruir") {
        // Si el destructor fuera recursivo (un ~Nodo llamando al siguiente),
        // esto reventaria la pila.
        Lista<int> lista;
        for (int i = 0; i < 100'000; ++i) {
            lista.anadir(i);
        }
        CHECK(lista.tam() == 100'000);
        CHECK(std::ranges::distance(lista) == 100'000);
    }

    SECTION("funciona con tipos que no son enteros") {
        Lista<std::string> textos;
        textos.anadir("uno");
        textos.anadir("dos");

        CHECK(textos.tam() == 2);
        CHECK(textos.primero() == "uno");
        CHECK(textos.begin()->size() == 3);  // operator->
    }
}

TEST_CASE("sacar_primero desengancha por delante", "[iteradores][sacar]") {
    Lista<int> lista = lista_de({1, 2, 3});

    SECTION("saca en orden") {
        CHECK(lista.sacar_primero() == 1);
        CHECK(lista.tam() == 2);
        CHECK(lista.sacar_primero() == 2);
        CHECK(lista.sacar_primero() == 3);
        CHECK(lista.vacia());
    }

    SECTION("sacar de una lista vacia es un error") {
        Lista<int> vacia;
        CHECK_THROWS_AS(vacia.sacar_primero(), std::out_of_range);
        CHECK_THROWS_AS(vacia.primero(), std::out_of_range);
    }

    SECTION("vaciar del todo y volver a llenar") {
        // Si sacar_primero no pone `ultimo_` a nullptr al quedarse vacia, el
        // siguiente anadir escribe en un nodo ya destruido.
        while (!lista.vacia()) {
            (void)lista.sacar_primero();
        }
        REQUIRE(lista.vacia());

        lista.anadir(10);
        lista.anadir(20);
        CHECK(lista.tam() == 2);
        CHECK(recorrer(lista) == std::vector<int>{10, 20});
    }
}

TEST_CASE("el recorrido funciona en las dos variantes", "[iteradores][recorrido]") {
    Lista<int> lista = lista_de({5, 10, 15});

    SECTION("for de rango sobre una lista mutable") {
        for (int& valor : lista) {
            valor *= 2;
        }
        CHECK(recorrer(lista) == std::vector<int>{10, 20, 30});
    }

    SECTION("for de rango sobre una lista constante") {
        const Lista<int>& constante = lista;
        int suma = 0;
        for (const int valor : constante) {
            suma += valor;
        }
        CHECK(suma == 30);
    }

    SECTION("incremento de prefijo y de sufijo") {
        auto it = lista.begin();
        CHECK(*it == 5);

        // El de sufijo devuelve el valor ANTERIOR.
        const auto copia = it++;
        CHECK(*copia == 5);
        CHECK(*it == 10);

        // El de prefijo devuelve el nuevo.
        const auto siguiente = ++it;
        CHECK(*siguiente == 15);
        CHECK(*it == 15);

        ++it;
        CHECK(it == lista.end());
    }

    SECTION("un iterator se convierte en const_iterator") {
        const Lista<int>::iterator it = lista.begin();
        const Lista<int>::const_iterator cit = it;
        CHECK(*cit == 5);
        CHECK(cit == lista.cbegin());
    }
}

TEST_CASE("la Lista funciona con los algoritmos estandar", "[iteradores][algoritmos]") {
    Lista<int> lista = lista_de({3, 1, 4, 1, 5, 9, 2, 6});

    SECTION("std::find") {
        CHECK(std::find(lista.begin(), lista.end(), 5) != lista.end());
        CHECK(std::find(lista.begin(), lista.end(), 99) == lista.end());
    }

    SECTION("std::count_if y std::accumulate") {
        CHECK(std::count_if(lista.begin(), lista.end(), [](int v) { return v > 3; }) == 4);
        CHECK(std::accumulate(lista.begin(), lista.end(), 0) == 31);
    }

    SECTION("std::ranges acepta la lista entera") {
        CHECK(std::ranges::distance(lista) == 8);
        CHECK(std::ranges::count(lista, 1) == 2);
        CHECK(*std::ranges::max_element(lista) == 9);
    }

    SECTION("y las vistas tambien") {
        auto vista = lista | std::views::filter([](int v) { return v % 2 == 0; }) |
                     std::views::transform([](int v) { return v * 10; });

        std::vector<int> resultado;
        std::ranges::copy(vista, std::back_inserter(resultado));
        CHECK(resultado == std::vector<int>{40, 20, 60});
    }

    SECTION("std::ranges::for_each modifica en sitio") {
        std::ranges::for_each(lista, [](int& valor) { valor = -valor; });
        CHECK(recorrer(lista) == std::vector<int>{-3, -1, -4, -1, -5, -9, -2, -6});
    }
}

TEST_CASE("mover una lista traspasa la cadena entera", "[iteradores][movimiento]") {
    SECTION("constructor de movimiento") {
        Lista<int> origen = lista_de({1, 2, 3});
        const Lista<int> destino = std::move(origen);

        CHECK(destino.tam() == 3);
        CHECK(recorrer(destino) == std::vector<int>{1, 2, 3});
        CHECK(origen.vacia());  // NOLINT(bugprone-use-after-move)
        CHECK(origen.tam() == 0);
    }

    SECTION("asignacion por movimiento") {
        Lista<int> origen = lista_de({7, 8});
        Lista<int> destino = lista_de({1, 2, 3, 4, 5});

        destino = std::move(origen);

        CHECK(destino.tam() == 2);
        CHECK(recorrer(destino) == std::vector<int>{7, 8});
        CHECK(origen.vacia());  // NOLINT(bugprone-use-after-move)
    }

    SECTION("una lista movida se puede reutilizar") {
        Lista<int> origen = lista_de({1, 2});
        const Lista<int> destino = std::move(origen);

        origen.anadir(99);  // NOLINT(bugprone-use-after-move)
        CHECK(origen.tam() == 1);
        CHECK(recorrer(origen) == std::vector<int>{99});
    }
}

TEST_CASE("limpiar vacia la lista sin recursion", "[iteradores][limpiar]") {
    Lista<int> lista;
    for (int i = 0; i < 50'000; ++i) {
        lista.anadir(i);
    }
    REQUIRE(lista.tam() == 50'000);

    lista.limpiar();

    CHECK(lista.vacia());
    CHECK(lista.tam() == 0);
    CHECK(lista.begin() == lista.end());

    SECTION("y se puede volver a usar") {
        lista.anadir(1);
        CHECK(lista.tam() == 1);
        CHECK(lista.primero() == 1);
    }
}
