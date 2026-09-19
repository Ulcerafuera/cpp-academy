#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

/// Leccion 11: escribir un contenedor que la biblioteca estandar reconozca.
///
/// Los TODO de esta leccion estan repartidos entre la clase Lista y su clase
/// anidada Iterador. El bloque de alias del iterador (iterator_category,
/// value_type, difference_type...) ya esta puesto: leelo antes de empezar,
/// porque es la parte que hace que <algorithm> y <ranges> reconozcan el tipo.
///
/// Un tipo se integra con la STL no por heredar de nada, sino por cumplir un
/// contrato: exponer `begin()` y `end()`, y que lo que devuelven se comporte
/// como un iterador. A partir de ahi funcionan el `for` de rango, los
/// algoritmos de <algorithm> y las vistas de <ranges>.
namespace academy::iteradores {

/// Lista enlazada simple que posee sus nodos.
///
/// Es de solo-movimiento a proposito: los nodos se encadenan con
/// `std::unique_ptr`, que no se puede copiar. Copiar la lista exigiria
/// duplicar la cadena entera, y aqui interesa el iterador, no eso.
template<typename T>
class Lista {
private:
    struct Nodo {
        explicit Nodo(T valor) : dato(std::move(valor)) {}

        T dato;
        std::unique_ptr<Nodo> siguiente;
    };

public:
    /// Iterador de avance sobre los elementos de la lista.
    ///
    /// La plantilla se instancia dos veces: con `T` para `iterator` y con
    /// `const T` para `const_iterator`, de modo que no hay codigo duplicado.
    template<typename Valor>
    class Iterador {
    public:
        // Estos cinco alias son lo que <iterator> y <ranges> consultan para
        // saber que sabe hacer este iterador. Sin ellos, std::find y el `for`
        // de rango ni siquiera compilan.
        using iterator_category = std::forward_iterator_tag;
        using iterator_concept = std::forward_iterator_tag;
        using value_type = std::remove_const_t<Valor>;
        using difference_type = std::ptrdiff_t;
        using pointer = Valor*;
        using reference = Valor&;

        // Un forward_iterator tiene que ser construible por defecto: ese es el
        // iterador "singular", que no apunta a ningun sitio.
        Iterador() = default;

        explicit Iterador(Nodo* nodo) noexcept : nodo_(nodo) {}

        /// Conversion de iterator a const_iterator (pero no al reves).
        ///
        /// La restriccion `requires` quita esta funcion cuando Valor ya es
        /// const: si no, seria una conversion de un tipo a si mismo.
        // NOLINTNEXTLINE(google-explicit-constructor)
        operator Iterador<const Valor>() const noexcept
            requires(!std::is_const_v<Valor>)
        {
            return Iterador<const Valor>(nodo_);
        }

        [[nodiscard]] reference operator*() const noexcept { return nodo_->dato; }
        [[nodiscard]] pointer operator->() const noexcept { return &nodo_->dato; }

        Iterador& operator++() noexcept {
            // TODO(11): avanza al siguiente nodo.
            // `nodo_->siguiente` es un unique_ptr; el iterador solo OBSERVA,
            // asi que hay que quedarse con su .get(), no con el unique_ptr.
            nodo_ = nullptr;
            return *this;
        }

        Iterador operator++(int) noexcept {
            // TODO(11): la version de sufijo devuelve el valor ANTERIOR.
            // Por eso la de prefijo es la que se prefiere en los bucles: esta
            // tiene que hacer una copia.
            Iterador copia = *this;
            return copia;
        }

        [[nodiscard]] friend bool operator==(const Iterador& izquierda,
                                             const Iterador& derecha) noexcept {
            // TODO(11): dos iteradores son iguales si apuntan al mismo nodo.
            // El iterador de final es el que apunta a nullptr.
            (void)izquierda;
            (void)derecha;
            return true;
        }

    private:
        friend class Lista;

        Nodo* nodo_ = nullptr;
    };

    using value_type = T;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    using iterator = Iterador<T>;
    using const_iterator = Iterador<const T>;

    Lista() = default;

    Lista(const Lista&) = delete;
    Lista& operator=(const Lista&) = delete;

    Lista(Lista&& otra) noexcept
        : primero_(std::move(otra.primero_)),
          ultimo_(std::exchange(otra.ultimo_, nullptr)),
          tam_(std::exchange(otra.tam_, 0)) {}

    Lista& operator=(Lista&& otra) noexcept {
        if (this != &otra) {
            limpiar();
            primero_ = std::move(otra.primero_);
            ultimo_ = std::exchange(otra.ultimo_, nullptr);
            tam_ = std::exchange(otra.tam_, 0);
        }
        return *this;
    }

    ~Lista() { limpiar(); }

    /// Anade @p valor al final. O(1) gracias al puntero al ultimo nodo.
    void anadir(T valor);

    /// Quita el primer elemento y lo devuelve.
    /// @throws std::out_of_range si la lista esta vacia.
    T sacar_primero();

    /// Vacia la lista sin recursion.
    void limpiar() noexcept;

    [[nodiscard]] size_type tam() const noexcept { return tam_; }
    [[nodiscard]] bool vacia() const noexcept { return tam_ == 0; }

    /// @throws std::out_of_range si la lista esta vacia.
    [[nodiscard]] reference primero();
    [[nodiscard]] const_reference primero() const;

    [[nodiscard]] iterator begin() noexcept {
        // TODO(11): el iterador inicial apunta al primer nodo.
        return iterator(nullptr);
    }
    [[nodiscard]] iterator end() noexcept { return iterator(nullptr); }
    [[nodiscard]] const_iterator begin() const noexcept {
        // TODO(11): igual que la anterior, pero sin permitir modificar.
        return const_iterator(nullptr);
    }
    [[nodiscard]] const_iterator end() const noexcept { return const_iterator(nullptr); }
    [[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] const_iterator cend() const noexcept { return end(); }

private:
    std::unique_ptr<Nodo> primero_;
    Nodo* ultimo_ = nullptr;
    size_type tam_ = 0;
};

// --- Implementacion ---------------------------------------------------------

template<typename T>
void Lista<T>::anadir(T valor) {
    // TODO(11): engancha un nodo nuevo al final, en tiempo constante.
    //
    // Guion:
    //   1. crea el nodo con std::make_unique<Nodo>(std::move(valor));
    //   2. guarda su puntero CRUDO antes de mover el unique_ptr: despues del
    //      move ya no podras preguntarselo;
    //   3. si la lista estaba vacia, el nodo nuevo es `primero_`; si no, va en
    //      `ultimo_->siguiente`;
    //   4. actualiza `ultimo_` y `tam_`.
    //
    // Sin el puntero `ultimo_` habria que recorrer la lista entera en cada
    // insercion: O(n) en vez de O(1).
    (void)valor;
    throw std::logic_error("TODO: implementa Lista::anadir");
}

template<typename T>
T Lista<T>::sacar_primero() {
    if (primero_ == nullptr) {
        throw std::out_of_range("Lista::sacar_primero: la lista esta vacia");
    }

    // TODO(11): saca el dato del primer nodo, desengancha ese nodo y
    // devuelve el dato.
    //
    // Cuidado: si la lista se queda vacia hay que poner `ultimo_` a nullptr,
    // o apuntaria a un nodo ya destruido y el siguiente anadir() escribiria en
    // memoria liberada.
    throw std::logic_error("TODO: implementa Lista::sacar_primero");
}

template<typename T>
void Lista<T>::limpiar() noexcept {
    // Destruir `primero_` sin mas encadenaria ~Nodo -> ~unique_ptr -> ~Nodo...
    // una recursion tan profunda como la lista, que desborda la pila con
    // listas grandes. Por eso se desmonta la cadena en un bucle.
    while (primero_ != nullptr) {
        std::unique_ptr<Nodo> cabeza = std::move(primero_);
        primero_ = std::move(cabeza->siguiente);
    }
    ultimo_ = nullptr;
    tam_ = 0;
}

template<typename T>
typename Lista<T>::reference Lista<T>::primero() {
    if (primero_ == nullptr) {
        throw std::out_of_range("Lista::primero: la lista esta vacia");
    }
    return primero_->dato;
}

template<typename T>
typename Lista<T>::const_reference Lista<T>::primero() const {
    if (primero_ == nullptr) {
        throw std::out_of_range("Lista::primero: la lista esta vacia");
    }
    return primero_->dato;
}

}  // namespace academy::iteradores
