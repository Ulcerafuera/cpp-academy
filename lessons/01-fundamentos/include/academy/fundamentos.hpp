#pragma once

#include <string>

/// Leccion 01: tipos basicos, control de flujo, funciones y errores.
namespace academy::fundamentos {

/// Devuelve la representacion FizzBuzz de @p n.
///
/// - multiplo de 3 y de 5 -> "FizzBuzz"
/// - multiplo de 3        -> "Fizz"
/// - multiplo de 5        -> "Buzz"
/// - en cualquier otro caso, el numero en decimal ("7")
///
/// @throws std::invalid_argument si @p n < 1.
std::string fizzbuzz(int n);

/// Maximo comun divisor de @p a y @p b por el algoritmo de Euclides.
///
/// El resultado nunca es negativo. `mcd(0, 0)` vale 0 por convenio.
/// Precondicion: ni @p a ni @p b pueden ser `INT_MIN` (su valor absoluto no es
/// representable en un `int`).
int mcd(int a, int b);

/// Indica si @p n es un numero primo. Los valores menores que 2 no lo son.
bool es_primo(int n);

}  // namespace academy::fundamentos
