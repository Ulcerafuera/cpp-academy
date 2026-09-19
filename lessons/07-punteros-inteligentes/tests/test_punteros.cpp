#include "academy/punteros.hpp"

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

using academy::punteros::crear;
using academy::punteros::Inventario;
using academy::punteros::Nodo;
using academy::punteros::Recurso;

TEST_CASE("crear entrega la propiedad del recurso", "[punteros][unique]") {
    const int antes = Recurso::vivos();

    SECTION("el recurso existe mientras vive el unique_ptr") {
        {
            const std::unique_ptr<Recurso> recurso = crear("motor");
            REQUIRE(recurso != nullptr);
            CHECK(recurso->nombre() == "motor");
            CHECK(Recurso::vivos() == antes + 1);
        }
        // Al salir del ambito se destruye solo: no hay ningun delete a la vista.
        CHECK(Recurso::vivos() == antes);
    }

    SECTION("mover transfiere la propiedad sin duplicar el recurso") {
        std::unique_ptr<Recurso> origen = crear("bomba");
        REQUIRE(origen != nullptr);

        const std::unique_ptr<Recurso> destino = std::move(origen);

        CHECK(destino != nullptr);
        CHECK(destino->nombre() == "bomba");
        CHECK(origen == nullptr);  // NOLINT(bugprone-use-after-move)
        CHECK(Recurso::vivos() == antes + 1);
    }

    SECTION("reasignar destruye lo anterior") {
        std::unique_ptr<Recurso> recurso = crear("uno");
        CHECK(Recurso::vivos() == antes + 1);

        recurso = crear("dos");
        CHECK(Recurso::vivos() == antes + 1);
        CHECK(recurso->nombre() == "dos");
    }
}

TEST_CASE("el Inventario posee sus recursos", "[punteros][inventario]") {
    const int antes = Recurso::vivos();

    SECTION("anadir y contar") {
        Inventario inventario;
        CHECK(inventario.vacio());

        inventario.anadir(crear("martillo"));
        inventario.anadir(crear("sierra"));

        CHECK(inventario.tam() == 2);
        CHECK_FALSE(inventario.vacio());
        CHECK(Recurso::vivos() == antes + 2);
    }

    SECTION("al destruirse el inventario se lleva sus recursos") {
        {
            Inventario inventario;
            inventario.anadir(crear("a"));
            inventario.anadir(crear("b"));
            inventario.anadir(crear("c"));
            REQUIRE(Recurso::vivos() == antes + 3);
        }
        CHECK(Recurso::vivos() == antes);
    }

    SECTION("anadir un nulo no hace nada") {
        Inventario inventario;
        inventario.anadir(nullptr);
        CHECK(inventario.vacio());
    }
}

TEST_CASE("buscar presta el recurso sin cederlo", "[punteros][inventario][buscar]") {
    Inventario inventario;
    inventario.anadir(crear("llave"));
    inventario.anadir(crear("destornillador"));

    SECTION("encuentra lo que hay") {
        const Recurso* encontrado = inventario.buscar("llave");
        REQUIRE(encontrado != nullptr);
        CHECK(encontrado->nombre() == "llave");
        // El inventario sigue siendo el dueno: no ha perdido nada.
        CHECK(inventario.tam() == 2);
    }

    SECTION("devuelve nullptr si no esta") {
        CHECK(inventario.buscar("taladro") == nullptr);
        CHECK(inventario.buscar("") == nullptr);
    }

    SECTION("la version const no permite modificar") {
        const Inventario& constante = inventario;
        const Recurso* encontrado = constante.buscar("destornillador");
        REQUIRE(encontrado != nullptr);
        CHECK(encontrado->nombre() == "destornillador");
    }
}

TEST_CASE("extraer transfiere la propiedad al llamante", "[punteros][inventario][extraer]") {
    const int antes = Recurso::vivos();

    Inventario inventario;
    inventario.anadir(crear("x"));
    inventario.anadir(crear("y"));
    REQUIRE(Recurso::vivos() == antes + 2);

    SECTION("el recurso sale del inventario pero sigue vivo") {
        std::unique_ptr<Recurso> extraido = inventario.extraer("x");

        REQUIRE(extraido != nullptr);
        CHECK(extraido->nombre() == "x");
        CHECK(inventario.tam() == 1);
        CHECK(inventario.buscar("x") == nullptr);
        CHECK(Recurso::vivos() == antes + 2);  // nadie lo ha destruido

        extraido.reset();
        CHECK(Recurso::vivos() == antes + 1);
    }

    SECTION("extraer lo que no esta devuelve nullptr") {
        CHECK(inventario.extraer("z") == nullptr);
        CHECK(inventario.tam() == 2);
    }

    SECTION("vaciar el inventario extrayendo") {
        CHECK(inventario.extraer("x") != nullptr);
        CHECK(inventario.extraer("y") != nullptr);
        CHECK(inventario.vacio());
        CHECK(Recurso::vivos() == antes);
    }
}

TEST_CASE("el arbol enlaza padres e hijos", "[punteros][shared][arbol]") {
    auto raiz = std::make_shared<Nodo>("raiz");
    auto rama = std::make_shared<Nodo>("rama");
    auto hoja = std::make_shared<Nodo>("hoja");

    raiz->anadir_hijo(rama);
    rama->anadir_hijo(hoja);

    SECTION("los hijos quedan colgados del padre") {
        REQUIRE(raiz->hijos().size() == 1);
        CHECK(raiz->hijos().front()->nombre() == "rama");
        REQUIRE(rama->hijos().size() == 1);
        CHECK(rama->hijos().front()->nombre() == "hoja");
        CHECK(hoja->hijos().empty());
    }

    SECTION("el enlace al padre funciona en los dos sentidos") {
        CHECK(raiz->padre() == nullptr);
        CHECK(rama->padre() == raiz);
        CHECK(hoja->padre() == rama);
    }

    SECTION("profundidad") {
        CHECK(raiz->profundidad() == 0);
        CHECK(rama->profundidad() == 1);
        CHECK(hoja->profundidad() == 2);
    }

    SECTION("ruta") {
        CHECK(raiz->ruta() == "raiz");
        CHECK(rama->ruta() == "raiz/rama");
        CHECK(hoja->ruta() == "raiz/rama/hoja");
    }

    SECTION("anadir un hijo nulo no hace nada") {
        const std::size_t antes_hijos = raiz->hijos().size();
        raiz->anadir_hijo(nullptr);
        CHECK(raiz->hijos().size() == antes_hijos);
    }

    SECTION("varios hijos del mismo padre") {
        raiz->anadir_hijo(std::make_shared<Nodo>("otra"));
        REQUIRE(raiz->hijos().size() == 2);
        CHECK(raiz->hijos()[1]->ruta() == "raiz/otra");
    }
}

TEST_CASE("el arbol no tiene ciclos de referencia", "[punteros][weak][ciclos]") {
    const int antes = Nodo::vivos();

    SECTION("soltar la raiz destruye el arbol entero") {
        {
            auto raiz = std::make_shared<Nodo>("a");
            auto hijo = std::make_shared<Nodo>("b");
            auto nieto = std::make_shared<Nodo>("c");
            raiz->anadir_hijo(hijo);
            hijo->anadir_hijo(nieto);
            REQUIRE(Nodo::vivos() == antes + 3);
        }
        // Si el enlace al padre fuera shared_ptr en vez de weak_ptr, padre e
        // hijo se sostendrian mutuamente y aqui seguirian los tres vivos.
        CHECK(Nodo::vivos() == antes);
    }

    SECTION("un hijo puede sobrevivir a su padre") {
        std::shared_ptr<Nodo> hijo;
        {
            auto raiz = std::make_shared<Nodo>("efimera");
            hijo = std::make_shared<Nodo>("superviviente");
            raiz->anadir_hijo(hijo);
            REQUIRE(hijo->padre() == raiz);
            REQUIRE(Nodo::vivos() == antes + 2);
        }

        // La raiz ya no existe: el weak_ptr caduca y padre() lo dice.
        CHECK(Nodo::vivos() == antes + 1);
        CHECK(hijo->padre() == nullptr);
        CHECK(hijo->profundidad() == 0);
        CHECK(hijo->ruta() == "superviviente");
    }

    SECTION("el recuento de referencias es el esperado") {
        auto raiz = std::make_shared<Nodo>("r");
        auto hijo = std::make_shared<Nodo>("h");
        CHECK(hijo.use_count() == 1);

        raiz->anadir_hijo(hijo);
        // Ahora hay dos duenos del hijo: la variable local y el vector del
        // padre. El weak_ptr del hijo hacia el padre NO cuenta.
        CHECK(hijo.use_count() == 2);
        CHECK(raiz.use_count() == 1);
    }
}
