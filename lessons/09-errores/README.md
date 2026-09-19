# 09 · Errores: `optional`, `variant` y excepciones

Decir «no hay valor» y «algo ha salido mal» con el sistema de tipos, en vez de
con un `-1`, un `nullptr` o un `NaN`.

## Las tres herramientas

| Herramienta | Cuándo | Qué obliga al llamante |
|---|---|---|
| `std::optional<T>` | la ausencia es normal y esperable | a preguntar antes de usar el valor |
| `std::variant<T, Error>` | el fallo lleva información útil | a mirar cuál de las dos alternativas hay |
| excepción | el fallo es raro y se arregla más arriba | nada: se propaga sola |

El error clásico es usar un **valor centinela**: `-1` para «no encontrado`,
`NaN` para «no se pudo calcular». Funciona hasta que `-1` es un valor legítimo,
y siempre se puede ignorar por descuido.

## Interfaz a implementar

```cpp
std::optional<int>    a_entero(std::string_view texto);
std::optional<double> media(const std::vector<int>& valores);
std::optional<int>    primero_mayor_que(const std::vector<int>&, int limite);

using Resultado = std::variant<double, Error>;
Resultado   dividir(double numerador, double denominador);
Resultado   evaluar(std::string_view expresion);   // "2 + 2"
double      valor_o(const Resultado&, double alternativa);
std::string describir(const Resultado&);           // con std::visit
bool        es_valor(const Resultado&) noexcept;
Recuento    resumir(const std::vector<Resultado>&);
```

`a_real()` y el idioma `Casos` ya están escritos: el primero como modelo de
`a_entero`, el segundo para que lo uses en `describir`.

## Trampas que los tests comprueban

1. **`from_chars` tiene dos formas de fallar.** No basta con mirar el código de
   error: hay que comprobar **también** que se consumió todo el texto. Si no,
   `"42abc"` se analiza como `42` y el sufijo se pierde en silencio.
2. **La media no es una división entera.** `(1 + 2) / 2` con enteros vale `1`.
   Y la suma hay que acumularla en `long long`, o desborda.
3. **`std::get` lanza, `std::get_if` no.** Para preguntar «¿qué hay aquí?» se
   usa `get_if` (devuelve un puntero o `nullptr`) o `holds_alternative`.
4. **`std::to_string(2.0)` es `"2.000000"`.** Para `"2"` hay que usar
   `std::ostringstream`, que aplica el formato por defecto de `operator<<`.
5. **La división por cero se propaga.** `evaluar("1 / 0")` tiene que devolver
   el mismo error que `dividir`, no uno nuevo.

## `std::visit` y el idioma `Casos`

```cpp
std::visit(Casos{[](double valor)       { /* ... */ },
                 [](const Error& error) { /* ... */ }},
           resultado);
```

`Casos` hereda de todos los lambdas y hace visibles sus `operator()`, de modo
que la sobrecarga elige el correcto. La gracia de `std::visit` es que exige un
caso para **cada** alternativa: si mañana el `variant` gana un tercer tipo, esto
deja de **compilar** en vez de fallar en ejecución.

## `value_or` y compañía

```cpp
media(valores).value_or(0.0);   // el valor, o 0.0 si no hay
*media(valores);                // sin comprobar: UB si está vacío
media(valores).value();         // lanza std::bad_optional_access si está vacío
```

`operator*` es el rápido y el peligroso; `value()` es el seguro.

## Ejecutar

```bash
ctest --preset ejercicios -R "errores"
./build/ejercicios/lessons/09-errores/ejemplo_demo_errores
```
