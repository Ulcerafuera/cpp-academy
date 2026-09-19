#pragma once

#include <cstddef>

/// Leccion 03: clases, RAII y la regla de cinco.
namespace academy::raii {

/// Buffer de enteros que posee su propia memoria.
///
/// Es un `std::vector<int>` en miniatura, escrito a mano para practicar las
/// cinco operaciones especiales. Invariante de la clase:
///
///   - si `tam_ == 0` entonces `datos_ == nullptr`
///   - si `tam_ > 0`  entonces `datos_` apunta a un bloque de `tam_` enteros
///     reservado con `new[]` y propiedad exclusiva de este objeto
class Buffer {
public:
    /// Buffer vacio. No reserva memoria.
    Buffer() noexcept = default;

    /// Buffer de @p n enteros, todos a cero.
    explicit Buffer(std::size_t n);

    /// Copia profunda: el nuevo buffer tiene su propio bloque.
    Buffer(const Buffer& otro);
    Buffer& operator=(const Buffer& otro);

    /// Movimiento: roba el bloque y deja @p otro vacio pero utilizable.
    Buffer(Buffer&& otro) noexcept;
    Buffer& operator=(Buffer&& otro) noexcept;

    ~Buffer();

    [[nodiscard]] std::size_t tam() const noexcept { return tam_; }
    [[nodiscard]] bool vacio() const noexcept { return tam_ == 0; }

    [[nodiscard]] int* datos() noexcept { return datos_; }
    [[nodiscard]] const int* datos() const noexcept { return datos_; }

    /// Acceso sin comprobar. Indice fuera de rango: comportamiento indefinido.
    [[nodiscard]] int& operator[](std::size_t i) noexcept { return datos_[i]; }
    [[nodiscard]] const int& operator[](std::size_t i) const noexcept { return datos_[i]; }

    /// Acceso comprobado.
    /// @throws std::out_of_range si @p i >= tam().
    [[nodiscard]] int& en(std::size_t i);
    [[nodiscard]] const int& en(std::size_t i) const;

    /// Intercambia el contenido de dos buffers sin copiar ni reservar.
    void intercambiar(Buffer& otro) noexcept;

private:
    int* datos_ = nullptr;
    std::size_t tam_ = 0;
};

/// Igualdad por contenido: mismo tamano y mismos elementos.
bool operator==(const Buffer& izquierda, const Buffer& derecha) noexcept;

}  // namespace academy::raii
