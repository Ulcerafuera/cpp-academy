#include "academy/figuras.hpp"

#include <algorithm>
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

// --- Circulo ----------------------------------------------------------------

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

// --- Rectangulo -------------------------------------------------------------

Rectangulo::Rectangulo(double ancho, double alto) : ancho_(ancho), alto_(alto) {
    exigir_positivo(ancho, "Rectangulo: el ancho");
    exigir_positivo(alto, "Rectangulo: el alto");
}

bool Rectangulo::es_cuadrado() const noexcept {
    return ancho_ == alto_;
}

double Rectangulo::area() const {
    return ancho_ * alto_;
}

double Rectangulo::perimetro() const {
    return 2.0 * (ancho_ + alto_);
}

std::string Rectangulo::nombre() const {
    return es_cuadrado() ? "cuadrado" : "rectangulo";
}

std::unique_ptr<Figura> Rectangulo::clonar() const {
    return std::make_unique<Rectangulo>(*this);
}

// --- Triangulo --------------------------------------------------------------

Triangulo::Triangulo(double a, double b, double c) : a_(a), b_(b), c_(c) {
    exigir_positivo(a, "Triangulo: el lado a");
    exigir_positivo(b, "Triangulo: el lado b");
    exigir_positivo(c, "Triangulo: el lado c");

    if (a + b <= c || a + c <= b || b + c <= a) {
        throw std::invalid_argument("Triangulo: los lados no cumplen la desigualdad triangular");
    }
}

double Triangulo::area() const {
    // Formula de Heron.
    const double s = perimetro() / 2.0;
    const double bajo_raiz = s * (s - a_) * (s - b_) * (s - c_);
    // El redondeo puede dejar el producto en un negativo minusculo para
    // triangulos casi degenerados; se recorta a cero antes de la raiz.
    return std::sqrt(std::max(bajo_raiz, 0.0));
}

double Triangulo::perimetro() const {
    return a_ + b_ + c_;
}

std::string Triangulo::nombre() const {
    return "triangulo";
}

std::unique_ptr<Figura> Triangulo::clonar() const {
    return std::make_unique<Triangulo>(*this);
}

// --- Operaciones sobre colecciones ------------------------------------------

double area_total(const Figuras& figuras) {
    double total = 0.0;
    for (const std::unique_ptr<Figura>& figura : figuras) {
        if (figura != nullptr) {
            // Aqui esta el despacho dinamico: la llamada es la misma para
            // todas, pero se ejecuta el area() del tipo real de cada objeto.
            total += figura->area();
        }
    }
    return total;
}

const Figura& mayor(const Figuras& figuras) {
    const Figura* mejor = nullptr;
    for (const std::unique_ptr<Figura>& figura : figuras) {
        if (figura != nullptr && (mejor == nullptr || figura->area() > mejor->area())) {
            mejor = figura.get();
        }
    }

    if (mejor == nullptr) {
        throw std::invalid_argument("mayor: no hay ninguna figura");
    }
    return *mejor;
}

Figuras clonar_todas(const Figuras& figuras) {
    Figuras copias;
    copias.reserve(figuras.size());
    for (const std::unique_ptr<Figura>& figura : figuras) {
        copias.push_back(figura != nullptr ? figura->clonar() : nullptr);
    }
    return copias;
}

std::vector<std::string> nombres(const Figuras& figuras) {
    std::vector<std::string> resultado;
    resultado.reserve(figuras.size());
    for (const std::unique_ptr<Figura>& figura : figuras) {
        if (figura != nullptr) {
            resultado.push_back(figura->nombre());
        }
    }
    return resultado;
}

}  // namespace academy::polimorfismo
