#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/// Leccion 07: propiedad y punteros inteligentes.
///
/// En la leccion 03 se escribio a mano la gestion de un recurso. Aqui se usa la
/// que trae la biblioteca estandar, y se responde a la pregunta que toda
/// interfaz en C++ tiene que contestar: *quien es el dueno de esto*.
///
///   std::unique_ptr<T>   un unico dueno; mover transfiere la propiedad
///   std::shared_ptr<T>   propiedad compartida con recuento de referencias
///   std::weak_ptr<T>     observador que no alarga la vida del objeto
///   T*  /  T&            observador prestado: NO es dueno de nada
namespace academy::punteros {

/// Recurso de mentira que lleva la cuenta de cuantos hay vivos.
///
/// No hay nada que implementar aqui: es el instrumento con el que los tests
/// comprueban que la propiedad funciona y que nada se queda sin destruir.
class Recurso {
public:
    explicit Recurso(std::string nombre);
    ~Recurso();

    Recurso(const Recurso&) = delete;
    Recurso& operator=(const Recurso&) = delete;
    Recurso(Recurso&&) = delete;
    Recurso& operator=(Recurso&&) = delete;

    [[nodiscard]] const std::string& nombre() const noexcept { return nombre_; }

    /// Cuantos Recurso existen en este momento.
    [[nodiscard]] static int vivos() noexcept;

private:
    std::string nombre_;
};

/// Crea un Recurso del que el llamante pasa a ser el unico dueno.
///
/// Devolver `std::unique_ptr` en vez de `Recurso*` hace imposible olvidarse de
/// destruirlo: el tipo de retorno documenta y obliga.
[[nodiscard]] std::unique_ptr<Recurso> crear(std::string nombre);

/// Coleccion que POSEE sus recursos.
class Inventario {
public:
    Inventario() = default;

    /// Toma la propiedad de @p recurso. Ignora los punteros nulos.
    void anadir(std::unique_ptr<Recurso> recurso);

    /// Busca por nombre y devuelve un observador PRESTADO, o `nullptr`.
    ///
    /// Devuelve un puntero crudo a proposito: quien lo recibe no es el dueno y
    /// el puntero deja de ser valido si el Inventario muere o se extrae ese
    /// recurso.
    [[nodiscard]] Recurso* buscar(std::string_view nombre) noexcept;
    [[nodiscard]] const Recurso* buscar(std::string_view nombre) const noexcept;

    /// Saca el recurso del inventario y TRANSFIERE su propiedad al llamante.
    /// Devuelve `nullptr` si no existe.
    [[nodiscard]] std::unique_ptr<Recurso> extraer(std::string_view nombre);

    [[nodiscard]] std::size_t tam() const noexcept { return recursos_.size(); }
    [[nodiscard]] bool vacio() const noexcept { return recursos_.empty(); }

private:
    std::vector<std::unique_ptr<Recurso>> recursos_;
};

/// Nodo de un arbol con enlace al padre.
///
/// El padre posee a sus hijos con `shared_ptr`, pero el hijo apunta al padre
/// con `weak_ptr`. Si ambos enlaces fueran `shared_ptr`, padre e hijo se
/// mantendrian vivos mutuamente para siempre: un ciclo de referencias, que es
/// una fuga que ni siquiera AddressSanitizer marca como error obvio.
class Nodo : public std::enable_shared_from_this<Nodo> {
public:
    explicit Nodo(std::string nombre);
    ~Nodo();

    [[nodiscard]] const std::string& nombre() const noexcept { return nombre_; }

    /// El padre, o `nullptr` si es la raiz o si el padre ya murio.
    [[nodiscard]] std::shared_ptr<Nodo> padre() const;

    [[nodiscard]] const std::vector<std::shared_ptr<Nodo>>& hijos() const noexcept {
        return hijos_;
    }

    /// Cuelga @p hijo de este nodo: lo anade a sus hijos y le apunta de vuelta.
    ///
    /// Precondicion: este nodo ya tiene que estar gestionado por un
    /// `shared_ptr` (creado con `std::make_shared<Nodo>(...)`). Si no,
    /// `shared_from_this()` lanza `std::bad_weak_ptr`.
    ///
    /// Ignora los hijos nulos.
    void anadir_hijo(std::shared_ptr<Nodo> hijo);

    /// Cuantos saltos hay hasta la raiz. La raiz esta a profundidad 0.
    [[nodiscard]] std::size_t profundidad() const;

    /// Ruta desde la raiz, con los nombres separados por '/'.
    [[nodiscard]] std::string ruta() const;

    /// Cuantos Nodo existen en este momento.
    [[nodiscard]] static int vivos() noexcept;

private:
    std::string nombre_;
    std::weak_ptr<Nodo> padre_;
    std::vector<std::shared_ptr<Nodo>> hijos_;
};

}  // namespace academy::punteros
