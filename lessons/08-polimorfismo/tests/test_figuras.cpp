#include "academy/figuras.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <memory>
#include <numbers>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

using academy::polimorfismo::area_total;
using academy::polimorfismo::Circulo;
using academy::polimorfismo::clonar_todas;
using academy::polimorfismo::Figura;
using academy::polimorfismo::Figuras;
using academy::polimorfismo::mayor;
using academy::polimorfismo::nombres;
using academy::polimorfismo::Rectangulo;
using academy::polimorfismo::Triangulo;

using Catch::Matchers::WithinRel;

namespace {

/// Figura de mentira que cuenta cuantas hay vivas. Sirve para comprobar que el
/// destructor virtual se llama al destruir por un puntero a la base.
class Espia final : public Figura {
public:
    Espia() { ++vivas_; }
    Espia(const Espia& otra) : Figura(otra) { ++vivas_; }
    ~Espia() override { --vivas_; }

    Espia& operator=(const Espia&) = delete;
    Espia(Espia&&) = delete;
    Espia& operator=(Espia&&) = delete;

    [[nodiscard]] double area() const override { return 1.0; }
    [[nodiscard]] double perimetro() const override { return 4.0; }
    [[nodiscard]] std::string nombre() const override { return "espia"; }
    [[nodiscard]] std::unique_ptr<Figura> clonar() const override {
        return std::make_unique<Espia>(*this);
    }

    static int vivas() noexcept { return vivas_; }

private:
    static int vivas_;
};

int Espia::vivas_ = 0;

/// Recibe la figura por la interfaz: no sabe ni le importa cual es el tipo real.
double doble_del_area(const Figura& figura) {
    return 2.0 * figura.area();
}

Figuras hacer_coleccion() {
    Figuras figuras;
    figuras.push_back(std::make_unique<Circulo>(2.0));
    figuras.push_back(std::make_unique<Rectangulo>(3.0, 4.0));
    figuras.push_back(std::make_unique<Triangulo>(3.0, 4.0, 5.0));
    return figuras;
}

}  // namespace

TEST_CASE("Figura es una interfaz", "[polimorfismo][interfaz]") {
    STATIC_REQUIRE(std::is_abstract_v<Figura>);
    // Sin destructor virtual, destruir por un puntero a la base seria
    // comportamiento indefinido.
    STATIC_REQUIRE(std::has_virtual_destructor_v<Figura>);
}

TEST_CASE("cada figura calcula su area y su perimetro", "[polimorfismo][geometria]") {
    SECTION("circulo") {
        const Circulo circulo(2.0);
        CHECK_THAT(circulo.area(), WithinRel(4.0 * std::numbers::pi, 1e-12));
        CHECK_THAT(circulo.perimetro(), WithinRel(4.0 * std::numbers::pi, 1e-12));
        CHECK(circulo.nombre() == "circulo");
        CHECK(circulo.radio() == 2.0);
    }

    SECTION("rectangulo") {
        const Rectangulo rectangulo(3.0, 4.0);
        CHECK_THAT(rectangulo.area(), WithinRel(12.0, 1e-12));
        CHECK_THAT(rectangulo.perimetro(), WithinRel(14.0, 1e-12));
        CHECK(rectangulo.nombre() == "rectangulo");
        CHECK_FALSE(rectangulo.es_cuadrado());
    }

    SECTION("un rectangulo de lados iguales se llama cuadrado") {
        const Rectangulo cuadrado(5.0, 5.0);
        CHECK(cuadrado.es_cuadrado());
        CHECK(cuadrado.nombre() == "cuadrado");
        CHECK_THAT(cuadrado.area(), WithinRel(25.0, 1e-12));
    }

    SECTION("triangulo rectangulo 3-4-5") {
        const Triangulo triangulo(3.0, 4.0, 5.0);
        CHECK_THAT(triangulo.area(), WithinRel(6.0, 1e-12));
        CHECK_THAT(triangulo.perimetro(), WithinRel(12.0, 1e-12));
        CHECK(triangulo.nombre() == "triangulo");
    }

    SECTION("triangulo equilatero de lado 2") {
        const Triangulo triangulo(2.0, 2.0, 2.0);
        CHECK_THAT(triangulo.area(), WithinRel(std::numbers::sqrt3, 1e-12));
    }

    SECTION("triangulo casi degenerado: el area no puede ser NaN") {
        const Triangulo triangulo(1.0, 1.0, 1.999999999);
        const double area = triangulo.area();
        CHECK(area == area);  // NaN != NaN
        CHECK(area >= 0.0);
    }
}

TEST_CASE("las figuras validan sus dimensiones", "[polimorfismo][errores]") {
    SECTION("dimensiones no positivas") {
        CHECK_THROWS_AS(Circulo(0.0), std::invalid_argument);
        CHECK_THROWS_AS(Circulo(-1.0), std::invalid_argument);
        CHECK_THROWS_AS(Rectangulo(0.0, 5.0), std::invalid_argument);
        CHECK_THROWS_AS(Rectangulo(5.0, -2.0), std::invalid_argument);
        CHECK_THROWS_AS(Triangulo(0.0, 4.0, 5.0), std::invalid_argument);
    }

    SECTION("la desigualdad triangular") {
        CHECK_THROWS_AS(Triangulo(1.0, 2.0, 10.0), std::invalid_argument);
        CHECK_THROWS_AS(Triangulo(10.0, 2.0, 1.0), std::invalid_argument);
        // Degenerado: 1 + 2 == 3, los tres puntos en linea recta.
        CHECK_THROWS_AS(Triangulo(1.0, 2.0, 3.0), std::invalid_argument);
    }

    SECTION("los casos validos no lanzan") {
        CHECK_NOTHROW(Circulo(0.001));
        CHECK_NOTHROW(Rectangulo(1.0, 1.0));
        CHECK_NOTHROW(Triangulo(2.0, 3.0, 4.0));
    }
}

TEST_CASE("el despacho dinamico elige la implementacion real", "[polimorfismo][despacho]") {
    const Circulo circulo(1.0);
    const Rectangulo rectangulo(2.0, 3.0);

    // La misma funcion, que solo conoce la interfaz, da resultados distintos
    // segun el tipo real que reciba.
    CHECK_THAT(doble_del_area(circulo), WithinRel(2.0 * std::numbers::pi, 1e-12));
    CHECK_THAT(doble_del_area(rectangulo), WithinRel(12.0, 1e-12));

    SECTION("a traves de un puntero a la base") {
        const std::unique_ptr<Figura> figura = std::make_unique<Rectangulo>(2.0, 3.0);
        CHECK(figura->nombre() == "rectangulo");
        CHECK_THAT(figura->area(), WithinRel(6.0, 1e-12));
    }
}

TEST_CASE("clonar conserva el tipo real", "[polimorfismo][clonar]") {
    SECTION("cada clon es del tipo del original") {
        const std::unique_ptr<Figura> original = std::make_unique<Triangulo>(3.0, 4.0, 5.0);
        const std::unique_ptr<Figura> copia = original->clonar();

        REQUIRE(copia != nullptr);
        CHECK(copia.get() != original.get());  // es otro objeto
        CHECK(copia->nombre() == "triangulo");
        // dynamic_cast solo tiene exito si el tipo dinamico es el que se pide.
        CHECK(dynamic_cast<const Triangulo*>(copia.get()) != nullptr);
        CHECK(dynamic_cast<const Circulo*>(copia.get()) == nullptr);
    }

    SECTION("el clon conserva los datos") {
        const std::unique_ptr<Figura> original = std::make_unique<Rectangulo>(7.0, 2.0);
        const std::unique_ptr<Figura> copia = original->clonar();

        const auto* como_rectangulo = dynamic_cast<const Rectangulo*>(copia.get());
        REQUIRE(como_rectangulo != nullptr);
        CHECK(como_rectangulo->ancho() == 7.0);
        CHECK(como_rectangulo->alto() == 2.0);
    }

    SECTION("clonar_todas hace una copia profunda de la coleccion") {
        const Figuras originales = hacer_coleccion();
        const Figuras copias = clonar_todas(originales);

        REQUIRE(copias.size() == originales.size());
        for (std::size_t i = 0; i < copias.size(); ++i) {
            CAPTURE(i);
            REQUIRE(copias[i] != nullptr);
            CHECK(copias[i].get() != originales[i].get());
            CHECK(copias[i]->nombre() == originales[i]->nombre());
            CHECK_THAT(copias[i]->area(), WithinRel(originales[i]->area(), 1e-12));
        }
    }
}

TEST_CASE("operaciones sobre una coleccion heterogenea", "[polimorfismo][coleccion]") {
    const Figuras figuras = hacer_coleccion();

    SECTION("area_total suma las tres") {
        const double esperado = 4.0 * std::numbers::pi + 12.0 + 6.0;
        CHECK_THAT(area_total(figuras), WithinRel(esperado, 1e-12));
    }

    SECTION("mayor devuelve la de mas area") {
        // circulo 12.566 > rectangulo 12 > triangulo 6
        CHECK(mayor(figuras).nombre() == "circulo");
    }

    SECTION("nombres respeta el orden") {
        const std::vector<std::string> esperado{"circulo", "rectangulo", "triangulo"};
        CHECK(nombres(figuras) == esperado);
    }

    SECTION("una coleccion vacia") {
        const Figuras vacia;
        CHECK_THAT(area_total(vacia), WithinRel(0.0, 1e-12));
        CHECK(nombres(vacia).empty());
        CHECK(clonar_todas(vacia).empty());
        CHECK_THROWS_AS(mayor(vacia), std::invalid_argument);
    }

    SECTION("los punteros nulos se ignoran") {
        Figuras con_huecos;
        con_huecos.push_back(nullptr);
        con_huecos.push_back(std::make_unique<Rectangulo>(2.0, 2.0));
        con_huecos.push_back(nullptr);

        CHECK_THAT(area_total(con_huecos), WithinRel(4.0, 1e-12));
        CHECK(nombres(con_huecos) == std::vector<std::string>{"cuadrado"});
        CHECK(mayor(con_huecos).nombre() == "cuadrado");
    }

    SECTION("solo nulos es como estar vacia") {
        Figuras solo_nulos;
        solo_nulos.push_back(nullptr);
        CHECK_THROWS_AS(mayor(solo_nulos), std::invalid_argument);
    }
}

TEST_CASE("el destructor virtual destruye el objeto completo", "[polimorfismo][destructor]") {
    REQUIRE(Espia::vivas() == 0);

    {
        // Se guarda como Figura, pero el objeto es un Espia. Al destruirse el
        // unique_ptr se llama a ~Figura()... que, por ser virtual, encuentra y
        // ejecuta ~Espia().
        const std::unique_ptr<Figura> figura = std::make_unique<Espia>();
        CHECK(Espia::vivas() == 1);
        CHECK(figura->nombre() == "espia");
    }

    CHECK(Espia::vivas() == 0);

    SECTION("tambien al clonar") {
        {
            const std::unique_ptr<Figura> original = std::make_unique<Espia>();
            const std::unique_ptr<Figura> copia = original->clonar();
            CHECK(Espia::vivas() == 2);
        }
        CHECK(Espia::vivas() == 0);
    }
}
