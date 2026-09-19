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
        // TODO(06): bloquea con std::lock_guard, comprueba `cerrada_`, encola
        // con std::move y notifica.
        //
        // Notifica FUERA del ambito del cerrojo (abre un bloque { } para el
        // lock_guard): si notificas con el mutex cogido, el hilo que despierta
        // se vuelve a dormir inmediatamente esperando ese mismo mutex.
        (void)valor;
        throw std::logic_error("TODO: implementa ColaSegura::poner");
    }

    /// Espera hasta que haya un elemento y lo extrae.
    /// @throws ColaCerrada si la cola se cierra estando vacia.
    T tomar() {
        // TODO(06): usa std::unique_lock y
        //   hay_datos_.wait(cerrojo, [this] { return !cola_.empty() || cerrada_; });
        //
        // El predicado NO es opcional: wait puede volver sin que nadie haya
        // notificado (despertar espurio), asi que la condicion se comprueba
        // siempre al despertar.
        //
        // Si al despertar la cola esta vacia, es que se ha cerrado: lanza
        // ColaCerrada.
        throw std::logic_error("TODO: implementa ColaSegura::tomar");
    }

    /// Extrae un elemento si lo hay, sin bloquear nunca.
    [[nodiscard]] std::optional<T> intentar_tomar() {
        // TODO(06): bloquea, y si la cola esta vacia devuelve std::nullopt.
        return std::nullopt;
    }

    /// Cierra la cola y despierta a todos los consumidores. Idempotente.
    void cerrar() {
        // TODO(06): marca `cerrada_` y despierta a TODOS con notify_all.
        // Con notify_one solo saldria uno de los consumidores y el resto se
        // quedaria dormido para siempre.
    }

    // Estos dos observadores ya estan hechos: sirven de referencia de como se
    // coge el cerrojo. Fijate en que son `const` y aun asi bloquean, gracias a
    // que `mutex_` esta declarado `mutable`.

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
