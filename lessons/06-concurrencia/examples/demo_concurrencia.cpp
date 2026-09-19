// Productor/consumidor con una cola bloqueante, y suma repartida entre hilos.

#include "academy/cola_segura.hpp"
#include "academy/paralelo.hpp"

#include <chrono>
#include <iostream>
#include <mutex>
#include <numeric>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {

std::mutex salida_mutex;

void escribir(const std::string& linea) {
    const std::lock_guard<std::mutex> cerrojo(salida_mutex);
    std::cout << linea << '\n';
}

void productor_consumidor() {
    escribir("-- Productor / consumidor --");

    academy::concurrencia::ColaSegura<int> cola;

    // std::jthread (C++20) se une solo al destruirse: RAII aplicado a un hilo.
    // Con std::thread habria que acordarse de llamar a join(), y si una
    // excepcion se saltara esa llamada, el destructor invocaria std::terminate.
    std::jthread consumidor([&cola] {
        // Una excepcion que se escape del cuerpo de un hilo termina el
        // programa entero, asi que aqui se capturan todas.
        try {
            while (true) {
                const int tarea = cola.tomar();
                escribir("  consumido: " + std::to_string(tarea));
            }
        } catch (const academy::concurrencia::ColaCerrada&) {
            escribir("  consumidor: la cola se cerro, termino");
        } catch (const std::exception& error) {
            escribir(std::string("  consumidor: ") + error.what());
        }
    });

    try {
        for (int i = 1; i <= 5; ++i) {
            cola.poner(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    } catch (const std::exception& error) {
        escribir(std::string("  productor: ") + error.what());
    }

    // Se cierra pase lo que pase: si no, el consumidor esperaria para siempre
    // y el destructor de jthread se quedaria bloqueado en el join.
    cola.cerrar();
}

void suma_repartida() {
    escribir("\n-- Suma repartida entre hilos --");

    std::vector<int> valores(1'000'000);
    std::iota(valores.begin(), valores.end(), 1);

    const unsigned hilos = academy::concurrencia::hilos_disponibles();
    escribir("  hilos disponibles: " + std::to_string(hilos));

    const auto inicio = std::chrono::steady_clock::now();
    const long long total = academy::concurrencia::suma_paralela(valores, hilos);
    const auto fin = std::chrono::steady_clock::now();

    const auto micros = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio);
    escribir("  suma  = " + std::to_string(total));
    escribir("  tardo = " + std::to_string(micros.count()) + " us");

    const long long esperado = 1'000'000LL * 1'000'001LL / 2;
    escribir(total == esperado ? "  coincide con la suma secuencial"
                               : "  NO coincide con la suma secuencial");
}

}  // namespace

int main() {
    try {
        productor_consumidor();
        suma_repartida();
    } catch (const std::logic_error& error) {
        std::cerr << "\n[pendiente] " << error.what() << '\n'
                  << "Rellena los TODO de lessons/06-concurrencia/exercises/ y vuelve a "
                     "compilar.\n";
        return 1;
    }
    return 0;
}
