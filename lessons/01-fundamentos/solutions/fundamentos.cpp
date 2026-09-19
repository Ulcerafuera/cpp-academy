#include "academy/fundamentos.hpp"

#include <stdexcept>
#include <string>

namespace academy::fundamentos {

std::string fizzbuzz(int n) {
    if (n < 1) {
        throw std::invalid_argument("fizzbuzz: n debe ser >= 1");
    }

    const bool tres = (n % 3 == 0);
    const bool cinco = (n % 5 == 0);

    if (tres && cinco) {
        return "FizzBuzz";
    }
    if (tres) {
        return "Fizz";
    }
    if (cinco) {
        return "Buzz";
    }
    return std::to_string(n);
}

int mcd(int a, int b) {
    // Se opera en long long para que negar INT_MIN no desborde durante el
    // calculo; la precondicion documentada garantiza que el resultado cabe.
    long long x = a < 0 ? -static_cast<long long>(a) : a;
    long long y = b < 0 ? -static_cast<long long>(b) : b;

    while (y != 0) {
        const long long resto = x % y;
        x = y;
        y = resto;
    }
    return static_cast<int>(x);
}

bool es_primo(int n) {
    if (n < 2) {
        return false;
    }
    if (n % 2 == 0) {
        return n == 2;
    }
    for (int divisor = 3; divisor <= n / divisor; divisor += 2) {
        if (n % divisor == 0) {
            return false;
        }
    }
    return true;
}

}  // namespace academy::fundamentos
