#pragma once

#include <memory>
#include <string>
#include <vector>

/// Leccion 08: herencia, funciones virtuales y despacho dinamico.
namespace academy::polimorfismo {

/// Interfaz de una figura plana.
///
/// Es una clase ABSTRACTA: tiene funciones virtuales puras (`= 0`), asi que no
/// se puede instanciar. Solo sirve para manejar por referencia o por puntero a
/// los objetos concretos que derivan de ella.
class Figura {
public:
    virtual ~Figura() = default;

    [[nodiscard]] virtual double area() const = 0;
    [[nodiscard]] virtual double perimetro() const = 0;
    [[nodiscard]] virtual std::string nombre() const = 0;

    /// Copia polimorfica: devuelve una copia del tipo DINAMICO real.
    ///
    /// Un constructor de copia no sirve aqui: `Figura copia = *figura;` ni
    /// siquiera compila (la clase es abstracta), y copiar a un `Rectangulo`
    /// desde una `const Figura&` perderia la parte derivada. Por eso la copia
    /// tiene que ser virtual.
    [[nodiscard]] virtual std::unique_ptr<Figura> clonar() const = 0;

protected:
    // Protegidos, no publicos: asi se puede copiar una Figura desde una clase
    // derivada (que es lo que hace clonar()) pero no rebanar una figura
    // concreta asignandola a una Figura suelta.
    Figura() = default;
    Figura(const Figura&) = default;
    Figura& operator=(const Figura&) = default;
    Figura(Figura&&) = default;
    Figura& operator=(Figura&&) = default;
};

/// Circulo de radio @p radio.
class Circulo final : public Figura {
public:
    /// @throws std::invalid_argument si @p radio <= 0.
    explicit Circulo(double radio);

    [[nodiscard]] double radio() const noexcept { return radio_; }

    [[nodiscard]] double area() const override;
    [[nodiscard]] double perimetro() const override;
    [[nodiscard]] std::string nombre() const override;
    [[nodiscard]] std::unique_ptr<Figura> clonar() const override;

private:
    double radio_;
};

/// Rectangulo de @p ancho por @p alto.
class Rectangulo final : public Figura {
public:
    /// @throws std::invalid_argument si alguna dimension es <= 0.
    Rectangulo(double ancho, double alto);

    [[nodiscard]] double ancho() const noexcept { return ancho_; }
    [[nodiscard]] double alto() const noexcept { return alto_; }
    [[nodiscard]] bool es_cuadrado() const noexcept;

    [[nodiscard]] double area() const override;
    [[nodiscard]] double perimetro() const override;
    /// Devuelve "cuadrado" si los dos lados son iguales, "rectangulo" si no.
    [[nodiscard]] std::string nombre() const override;
    [[nodiscard]] std::unique_ptr<Figura> clonar() const override;

private:
    double ancho_;
    double alto_;
};

/// Triangulo de lados @p a, @p b y @p c.
class Triangulo final : public Figura {
public:
    /// @throws std::invalid_argument si algun lado es <= 0 o si los tres no
    /// cumplen la desigualdad triangular (cada lado menor que la suma de los
    /// otros dos).
    Triangulo(double a, double b, double c);

    [[nodiscard]] double lado_a() const noexcept { return a_; }
    [[nodiscard]] double lado_b() const noexcept { return b_; }
    [[nodiscard]] double lado_c() const noexcept { return c_; }

    /// Area por la formula de Heron.
    [[nodiscard]] double area() const override;
    [[nodiscard]] double perimetro() const override;
    [[nodiscard]] std::string nombre() const override;
    [[nodiscard]] std::unique_ptr<Figura> clonar() const override;

private:
    double a_;
    double b_;
    double c_;
};

using Figuras = std::vector<std::unique_ptr<Figura>>;

/// Suma el area de todas las figuras. Los punteros nulos se ignoran.
[[nodiscard]] double area_total(const Figuras& figuras);

/// La figura de mayor area. Ante empates devuelve la primera.
/// @throws std::invalid_argument si no hay ninguna figura no nula.
[[nodiscard]] const Figura& mayor(const Figuras& figuras);

/// Copia profunda de toda la coleccion, conservando el tipo real de cada una.
[[nodiscard]] Figuras clonar_todas(const Figuras& figuras);

/// Nombres de las figuras, en el mismo orden. Los nulos se ignoran.
[[nodiscard]] std::vector<std::string> nombres(const Figuras& figuras);

}  // namespace academy::polimorfismo
