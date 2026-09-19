#include "academy/paralelo.hpp"

#include <algorithm>
#include <stdexcept>
#include <thread>

namespace academy::concurrencia {

long long suma_paralela(const std::vector<int>& valores, unsigned hilos) {
    if (hilos == 0) {
        throw std::invalid_argument("suma_paralela: hilos debe ser >= 1");
    }
    if (valores.empty()) {
        return 0;
    }

    const std::size_t total = valores.size();
    const std::size_t cuantos = std::min(static_cast<std::size_t>(hilos), total);

    // Cada hilo escribe en SU propia casilla: no hay dos hilos tocando la misma
    // posicion, asi que no hace falta mutex ni atomico. Un `long long` comun
    // con `+=` desde varios hilos seria una carrera de datos.
    std::vector<long long> parciales(cuantos, 0);
    std::vector<std::thread> trabajadores;
    trabajadores.reserve(cuantos);

    const std::size_t por_hilo = total / cuantos;
    const std::size_t sobrantes = total % cuantos;

    std::size_t inicio = 0;
    for (std::size_t i = 0; i < cuantos; ++i) {
        // Los primeros `sobrantes` hilos se llevan un elemento de mas, de modo
        // que el reparto no deja ninguno fuera.
        const std::size_t tamano = por_hilo + (i < sobrantes ? 1 : 0);
        const std::size_t fin = inicio + tamano;

        trabajadores.emplace_back([&valores, &parciales, i, inicio, fin] {
            long long acumulado = 0;
            for (std::size_t j = inicio; j < fin; ++j) {
                acumulado += valores[j];
            }
            parciales[i] = acumulado;
        });

        inicio = fin;
    }

    for (std::thread& trabajador : trabajadores) {
        trabajador.join();
    }

    long long total_final = 0;
    for (const long long parcial : parciales) {
        total_final += parcial;
    }
    return total_final;
}

unsigned hilos_disponibles() noexcept {
    const unsigned detectados = std::thread::hardware_concurrency();
    return detectados > 0 ? detectados : 1U;
}

}  // namespace academy::concurrencia
