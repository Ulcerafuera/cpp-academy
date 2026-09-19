#include "academy/punteros.hpp"

#include <algorithm>
#include <atomic>
#include <utility>

namespace academy::punteros {

namespace {

std::atomic<int> g_recursos_vivos{0};
std::atomic<int> g_nodos_vivos{0};

}  // namespace

// --- Recurso ----------------------------------------------------------------

Recurso::Recurso(std::string nombre) : nombre_(std::move(nombre)) {
    g_recursos_vivos.fetch_add(1, std::memory_order_relaxed);
}

Recurso::~Recurso() {
    g_recursos_vivos.fetch_sub(1, std::memory_order_relaxed);
}

int Recurso::vivos() noexcept {
    return g_recursos_vivos.load(std::memory_order_relaxed);
}

// --- crear ------------------------------------------------------------------

std::unique_ptr<Recurso> crear(std::string nombre) {
    // make_unique en vez de unique_ptr<Recurso>(new Recurso(...)): una sola
    // reserva, sin `new` suelto y a prueba de excepciones.
    return std::make_unique<Recurso>(std::move(nombre));
}

// --- Inventario -------------------------------------------------------------

void Inventario::anadir(std::unique_ptr<Recurso> recurso) {
    if (recurso == nullptr) {
        return;
    }
    // std::move: el unique_ptr no se puede copiar, solo transferir.
    recursos_.push_back(std::move(recurso));
}

Recurso* Inventario::buscar(std::string_view nombre) noexcept {
    // Se delega en la version const para no duplicar la busqueda. El
    // const_cast es seguro porque se sabe que el objeto de partida no es const.
    const Inventario& constante = *this;
    return const_cast<Recurso*>(constante.buscar(nombre));
}

const Recurso* Inventario::buscar(std::string_view nombre) const noexcept {
    const auto encontrado =
        std::find_if(recursos_.begin(), recursos_.end(),
                     [nombre](const std::unique_ptr<Recurso>& r) { return r->nombre() == nombre; });

    // .get() presta el puntero sin ceder la propiedad.
    return encontrado == recursos_.end() ? nullptr : encontrado->get();
}

std::unique_ptr<Recurso> Inventario::extraer(std::string_view nombre) {
    const auto encontrado =
        std::find_if(recursos_.begin(), recursos_.end(),
                     [nombre](const std::unique_ptr<Recurso>& r) { return r->nombre() == nombre; });

    if (encontrado == recursos_.end()) {
        return nullptr;
    }

    std::unique_ptr<Recurso> extraido = std::move(*encontrado);
    recursos_.erase(encontrado);
    return extraido;
}

// --- Nodo -------------------------------------------------------------------

Nodo::Nodo(std::string nombre) : nombre_(std::move(nombre)) {
    g_nodos_vivos.fetch_add(1, std::memory_order_relaxed);
}

Nodo::~Nodo() {
    g_nodos_vivos.fetch_sub(1, std::memory_order_relaxed);
}

std::shared_ptr<Nodo> Nodo::padre() const {
    // lock() convierte el weak_ptr en shared_ptr si el objeto sigue vivo, y en
    // nullptr si ya murio. No se puede desreferenciar un weak_ptr sin esto.
    return padre_.lock();
}

void Nodo::anadir_hijo(std::shared_ptr<Nodo> hijo) {
    if (hijo == nullptr) {
        return;
    }

    // shared_from_this() da un shared_ptr que COMPARTE el bloque de control
    // existente. Construir uno nuevo con shared_ptr<Nodo>(this) crearia un
    // segundo recuento independiente y el nodo se destruiria dos veces.
    hijo->padre_ = shared_from_this();
    hijos_.push_back(std::move(hijo));
}

std::size_t Nodo::profundidad() const {
    std::size_t saltos = 0;
    for (std::shared_ptr<Nodo> actual = padre(); actual != nullptr; actual = actual->padre()) {
        ++saltos;
    }
    return saltos;
}

std::string Nodo::ruta() const {
    std::string resultado = nombre_;
    for (std::shared_ptr<Nodo> actual = padre(); actual != nullptr; actual = actual->padre()) {
        resultado = actual->nombre() + '/' + resultado;
    }
    return resultado;
}

int Nodo::vivos() noexcept {
    return g_nodos_vivos.load(std::memory_order_relaxed);
}

}  // namespace academy::punteros
