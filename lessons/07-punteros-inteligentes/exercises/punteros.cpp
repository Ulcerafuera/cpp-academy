#include "academy/punteros.hpp"

#include <algorithm>
#include <atomic>
#include <stdexcept>
#include <utility>

namespace academy::punteros {

namespace {

std::atomic<int> g_recursos_vivos{0};
std::atomic<int> g_nodos_vivos{0};

}  // namespace

// --- Ya implementado: el instrumento de medida de los tests -----------------

Recurso::Recurso(std::string nombre) : nombre_(std::move(nombre)) {
    g_recursos_vivos.fetch_add(1, std::memory_order_relaxed);
}

Recurso::~Recurso() {
    g_recursos_vivos.fetch_sub(1, std::memory_order_relaxed);
}

int Recurso::vivos() noexcept {
    return g_recursos_vivos.load(std::memory_order_relaxed);
}

Nodo::Nodo(std::string nombre) : nombre_(std::move(nombre)) {
    g_nodos_vivos.fetch_add(1, std::memory_order_relaxed);
}

Nodo::~Nodo() {
    g_nodos_vivos.fetch_sub(1, std::memory_order_relaxed);
}

int Nodo::vivos() noexcept {
    return g_nodos_vivos.load(std::memory_order_relaxed);
}

// --- Te toca ----------------------------------------------------------------

std::unique_ptr<Recurso> crear(std::string nombre) {
    // TODO(07): devuelve un Recurso recien creado.
    // Usa std::make_unique, no `std::unique_ptr<Recurso>(new Recurso(...))`:
    // una sola reserva y ningun `new` suelto que se pueda quedar huerfano si
    // algo lanza por el camino.
    (void)nombre;
    throw std::logic_error("TODO: implementa academy::punteros::crear");
}

void Inventario::anadir(std::unique_ptr<Recurso> recurso) {
    // TODO(07): ignora los nulos y guarda el resto.
    // Un unique_ptr no se puede copiar: hay que MOVERLO al vector.
    (void)recurso;
    throw std::logic_error("TODO: implementa Inventario::anadir");
}

Recurso* Inventario::buscar(std::string_view nombre) noexcept {
    // TODO(07): busca por nombre y PRESTA el puntero con .get().
    // Devolver el unique_ptr seria ceder la propiedad; aqui solo se observa.
    (void)nombre;
    return nullptr;
}

const Recurso* Inventario::buscar(std::string_view nombre) const noexcept {
    // TODO(07): igual que la anterior, pero sin permitir modificar.
    (void)nombre;
    return nullptr;
}

std::unique_ptr<Recurso> Inventario::extraer(std::string_view nombre) {
    // TODO(07): saca el recurso del vector y devuelvelo.
    // Aqui SI se cede la propiedad: mueve el unique_ptr fuera del vector y
    // borra el hueco. Devuelve nullptr si no esta.
    (void)nombre;
    return nullptr;
}

std::shared_ptr<Nodo> Nodo::padre() const {
    // TODO(07): un weak_ptr no se puede desreferenciar directamente.
    // .lock() devuelve un shared_ptr si el objeto sigue vivo, o nullptr si ya
    // se destruyo.
    return nullptr;
}

void Nodo::anadir_hijo(std::shared_ptr<Nodo> hijo) {
    // TODO(07): anade `hijo` a `hijos_` y apunta su `padre_` a este nodo.
    //
    // CUIDADO: `hijo->padre_ = std::shared_ptr<Nodo>(this)` compila y esta
    // MAL. Crearia un bloque de control nuevo, independiente del que ya
    // gestiona este nodo, y el objeto acabaria destruyendose dos veces. Lo
    // correcto es shared_from_this(), que comparte el bloque existente.
    (void)hijo;
    throw std::logic_error("TODO: implementa Nodo::anadir_hijo");
}

std::size_t Nodo::profundidad() const {
    // TODO(07): cuenta los saltos hasta la raiz. La raiz esta a profundidad 0.
    return 0;
}

std::string Nodo::ruta() const {
    // TODO(07): sube hasta la raiz acumulando los nombres separados por '/'.
    // La ruta de la raiz es su propio nombre.
    return {};
}

}  // namespace academy::punteros
