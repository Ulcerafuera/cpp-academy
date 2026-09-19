# 08 · Herencia y polimorfismo

Interfaces abstractas, funciones virtuales, despacho dinámico y copia
polimórfica.

## Qué se practica

- Definir una interfaz con funciones virtuales puras (`= 0`).
- Implementarla en varias clases concretas con `override`.
- Manejar objetos distintos a través de una misma referencia o puntero.
- El idioma de **clonación virtual** para copiar sin conocer el tipo real.
- Por qué el destructor de una clase base polimórfica tiene que ser `virtual`.

## Interfaz a implementar

`Circulo` está entero como referencia de estilo. Te toca `Rectangulo`,
`Triangulo` y las cuatro funciones sobre colecciones:

```cpp
double area_total(const Figuras& figuras);
const Figura& mayor(const Figuras& figuras);
Figuras clonar_todas(const Figuras& figuras);
std::vector<std::string> nombres(const Figuras& figuras);
```

(`Figuras` es `std::vector<std::unique_ptr<Figura>>`.)

## Trampas que los tests comprueban

1. **El destructor virtual.** `std::unique_ptr<Figura>` llama a `~Figura()`.
   Si no fuera `virtual`, la parte derivada **no se destruiría**: comportamiento
   indefinido, y una fuga silenciosa. El test del `Espia` lo comprueba contando
   objetos vivos.
2. **La desigualdad triangular.** Con lados 1, 2 y 3 los tres puntos quedan en
   línea recta: no es un triángulo. Y con 1, 2 y 10 la fórmula de Herón daría la
   raíz de un número negativo, es decir `NaN`.
3. **`NaN` por redondeo.** En un triángulo casi degenerado,
   `s*(s-a)*(s-b)*(s-c)` puede salir negativo por error de coma flotante. Hay
   que recortarlo a cero antes de la raíz.
4. **`!(x > 0)` en vez de `x <= 0`.** Con `NaN`, *todas* las comparaciones son
   falsas, así que `x <= 0` lo deja pasar y `!(x > 0)` lo rechaza.
5. **`clonar()` tiene que construir el tipo derivado.** Los tests usan
   `dynamic_cast` para verificar que el clon de un `Triangulo` es realmente un
   `Triangulo`.

## Por qué `clonar()` y no el constructor de copia

Desde una `const Figura&` no se puede saber qué constructor llamar:

```cpp
void guardar(const Figura& f) {
    Figura copia = f;                      // no compila: Figura es abstracta
    auto copia = f.clonar();               // sí: cada clase sabe copiarse
}
```

Y asignar un objeto derivado a uno de la clase base produce **slicing**: se
copia solo la parte base y se pierde lo demás. Por eso `Figura` declara sus
operaciones de copia como `protected`: las clases derivadas pueden usarlas
(es lo que hace `clonar()`), pero nadie puede rebanar una figura desde fuera.

## `virtual`, `override`, `final`

- `virtual` en la base: esta función se puede redefinir.
- `override` en la derivada: **compruébame** que estoy redefiniendo algo que
  existe. Sin él, un error en la firma crea una función nueva en silencio y
  nunca se llama a la tuya.
- `final` en la clase: nadie hereda de aquí. Permite al compilador
  *devirtualizar* llamadas.

## Ejecutar

```bash
ctest --preset ejercicios -R "polimorfismo"
./build/ejercicios/lessons/08-polimorfismo/ejemplo_demo_polimorfismo
```
