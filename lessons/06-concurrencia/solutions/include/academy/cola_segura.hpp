#pragma once

#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <optional>
#include <queue>
#include <stdexcept>
#include <utility>

/// Leccion 06: hilos, exclusion mutua y variables de condicion.
namespace academy::concurrencia {

/// Se lanza al intentar tomar de una cola cerrada y vacia.
class ColaCerrada : public std::runtime_error {
public:
    ColaCerrada() : std::runtime_error("la cola esta cerrada y vacia") {}
};

/// Cola FIFO que varios hilos pueden usar a la vez.
///
/// Patron productor/consumidor: `poner` despierta a un consumidor bloqueado en
/// `tomar`. Para terminar ordenadamente se llama a `cerrar`, que despierta a
/// TODOS los consumidores; los que queden sin elementos reciben ColaCerrada.
template<typename T>
class ColaSegura {
public:
    ColaSegura() = default;

    // La clase posee un mutex, que no es copiable ni movible.
    ColaSegura(const ColaSegura&) = delete;
    ColaSegura& operator=(const ColaSegura&) = delete;
    ColaSegura(ColaSegura&&) = delete;
    ColaSegura& operator=(ColaSegura&&) = delete;

    ~ColaSegura() = default;

    /// Encola @p valor y despierta a un consumidor.
    /// @throws ColaCerrada si la cola ya se cerro.
    void poner(T valor) {
        {
            std::lock_guard<std::mutex> cerrojo(mutex_);
            if (cerrada_) {
                throw ColaCerrada{};
            }
            cola_.push(std::move(valor));
        }
        // Se notifica FUERA del cerrojo: si se hiciera dentro, el hilo
        // despertado tendria que volver a dormirse esperando el mutex.
        hay_datos_.notify_one();
    }

    /// Espera hasta que haya un elemento y lo extrae.
    /// @throws ColaCerrada si la cola se cierra estando vacia.
    T tomar() {
        std::unique_lock<std::mutex> cerrojo(mutex_);

        // El predicado protege de los despertares espurios: wait puede volver
        // sin que nadie haya notificado, asi que la condicion se vuelve a
        // comprobar siempre.
        hay_datos_.wait(cerrojo, [this] { return !cola_.empty() || cerrada_; });

        if (cola_.empty()) {
            throw ColaCerrada{};
        }

        T valor = std::move(cola_.front());
        cola_.pop();
        return valor;
    }

    /// Extrae un elemento si lo hay, sin bloquear nunca.
    [[nodiscard]] std::optional<T> intentar_tomar() {
        const std::lock_guard<std::mutex> cerrojo(mutex_);
        if (cola_.empty()) {
            return std::nullopt;
        }

        T valor = std::move(cola_.front());
        cola_.pop();
        return valor;
    }

    /// Cierra la cola y despierta a todos los consumidores. Idempotente.
    void cerrar() {
        {
            const std::lock_guard<std::mutex> cerrojo(mutex_);
            cerrada_ = true;
        }
        hay_datos_.notify_all();
    }

    [[nodiscard]] bool cerrada() const {
        const std::lock_guard<std::mutex> cerrojo(mutex_);
        return cerrada_;
    }

    [[nodiscard]] std::size_t tam() const {
        const std::lock_guard<std::mutex> cerrojo(mutex_);
        return cola_.size();
    }

    [[nodiscard]] bool vacia() const { return tam() == 0; }

private:
    // `mutable` porque los observadores son const pero necesitan bloquear.
    mutable std::mutex mutex_;
    std::condition_variable hay_datos_;
    std::queue<T> cola_;
    bool cerrada_ = false;
};

}  // namespace academy::concurrencia
