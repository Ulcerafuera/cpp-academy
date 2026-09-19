#include "academy/figuras.hpp"

#include <cmath>
#include <numbers>
#include <stdexcept>

namespace academy::polimorfismo {

namespace {

void exigir_positivo(double valor, const char* que) {
    if (!(valor > 0.0)) {  // con ! se rechaza tambien NaN
        throw std::invalid_argument(std::string(que) + " debe ser > 0");
    }
}

}  // namespace

// --- Ya implementado: el Circulo entero, como referencia de estilo ----------

Circulo::Circulo(double radio) : radio_(radio) {
    exigir_positivo(radio, "Circulo: el radio");
}

double Circulo::area() const {
    return std::numbers::pi * radio_ * radio_;
}

double Circulo::perimetro() const {
    return 2.0 * std::numbers::pi * radio_;
}

std::string Circulo::nombre() const {
    return "circulo";
}

std::unique_ptr<Figura> Circulo::clonar() const {
    // El tipo estatico devuelto es Figura, pero el objeto construido es un
    // Circulo: eso es lo que hace que la copia sea polimorfica.
    return std::make_unique<Circulo>(*this);
}

// --- Te toca: Rectangulo ----------------------------------------------------

Rectangulo::Rectangulo(double ancho, double alto) : ancho_(ancho), alto_(alto) {
    // TODO(08): valida las dos dimensiones con exigir_positivo().
}

bool Rectangulo::es_cuadrado() const noexcept {
    // TODO(08): dos lados iguales.
    return false;
}

double Rectangulo::area() const {
    // TODO(08)
    return 0.0;
}

double Rectangulo::perimetro() const {
    // TODO(08)
    return 0.0;
}

std::string Rectangulo::nombre() const {
    // TODO(08): "cuadrado" si los lados son iguales, "rectangulo" si no.
    return {};
}

std::unique_ptr<Figura> Rectangulo::clonar() const {
    // TODO(08): devuelve una copia de ESTE tipo, no de Figura.
    // Fijate en como lo hace Circulo::clonar().
    return nullptr;
}

// --- Te toca: Triangulo -----------------------------------------------------

Triangulo::Triangulo(double a, double b, double c) : a_(a), b_(b), c_(c) {
    // TODO(08): valida los tres lados y la desigualdad triangular:
    // cada lado tiene que ser menor que la suma de los otros dos.
}

double Triangulo::area() const {
    // TODO(08): formula de Heron.
    //   s = perimetro / 2
    //   area = raiz(s * (s-a) * (s-b) * (s-c))
    // El redondeo puede dejar el producto en un negativo minusculo para
    // triangulos casi degenerados: recortalo a cero antes de la raiz, o
    // std::sqrt te devolvera NaN.
    return 0.0;
}

double Triangulo::perimetro() const {
    // TODO(08)
    return 0.0;
}

std::string Triangulo::nombre() const {
    // TODO(08)
    return {};
}

std::unique_ptr<Figura> Triangulo::clonar() const {
    // TODO(08)
    return nullptr;
}

// --- Te toca: operaciones sobre colecciones ---------------------------------

double area_total(const Figuras& figuras) {
    // TODO(08): suma el area de todas, ignorando los punteros nulos.
    // La llamada `figura->area()` es la misma para todas, pero ejecuta el
    // area() del tipo REAL de cada objeto: eso es el despacho dinamico.
    (void)figuras;
    return 0.0;
}

const Figura& mayor(const Figuras& figuras) {
    // TODO(08): devuelve una REFERENCIA a la de mayor area.
    // Guarda un `const Figura*` con el mejor candidato mientras recorres y
    // desreferencialo al final; si no hay ninguna, lanza std::invalid_argument.
    (void)figuras;
    throw std::logic_error("TODO: implementa academy::polimorfismo::mayor");
}

Figuras clonar_todas(const Figuras& figuras) {
    // TODO(08): copia profunda de la coleccion. Cada elemento se copia con
    // clonar(), que conserva su tipo real.
    (void)figuras;
    return {};
}

std::vector<std::string> nombres(const Figuras& figuras) {
    // TODO(08)
    (void)figuras;
    return {};
}

}  // namespace academy::polimorfismo
