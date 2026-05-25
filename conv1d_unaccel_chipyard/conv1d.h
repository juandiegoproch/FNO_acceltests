/* conv1d.h — Conv1D K=1 (GEMV) en punto fijo Q15.16 para RV64.
 *
 * Port del baseline original (../conv1d_unaccel/conv1d.h, formato Q1.15 con
 * int16_t) a Q15.16 (int32_t, 16 bits fraccionarios). El formato se eligió
 * por coherencia con fft_accel/main.c:13 (FIXED_POINT_SHIFT=16).
 *
 * Layout (idéntico al baseline):
 *   x : [L][Cin]    int32_t en Q15.16   (valor real = entero / 2^16)
 *   W : [Cout][Cin] int32_t en Q15.16   (sólo K=1 ⇒ no hay dimensión K)
 *   y : [L][Cout]   int32_t en Q15.16
 *
 * Bias:
 *   bias : [Cout] int64_t en Q?.32, es decir, ya pre-escalado al ancho del
 *   acumulador (post-multiplicación, donde los fraccionarios se suman:
 *   16 + 16 = 32). Para inyectar un bias real `b`, usar `(int64_t)(b * 2^32)`.
 *   Si `bias == NULL`, se asume 0.
 *
 * Aritmética interna:
 *   - Cada producto x*W es int32×int32 promovido a int64 → Q?.32 en int64_t.
 *   - Acumulador int64_t (OBLIGATORIO; el baseline Q15 usaba int32, eso
 *     desborda en Q15.16 con Cin≥2 ya que un solo producto ocupa hasta 32
 *     bits de la palabra). Ver justificación en ../conv1d_unaccel/conv1d.c:155
 *     donde el acumulador era int32 — aquí se sube a int64.
 *   - Para volver a Q15.16: shift derecha de `shift_out = 16` con redondeo
 *     (sumar medio LSB antes del shift) y saturar a int32.
 */

#ifndef CONV1D_Q1516_H
#define CONV1D_Q1516_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Conv1D K=1 (pointwise), single sample, sin batch.
 * Matemáticamente: Y[l] = W · X[l] + b, con W (Cout, Cin), X[l] (Cin),
 * Y[l] (Cout). Es L productos matriz-vector independientes con la misma W.
 *
 *   x         : puntero a x[L][Cin]   (Q15.16, contiguo en ci dentro de l)
 *   L, Cin    : dimensiones de x
 *   W         : puntero a W[Cout][Cin] (Q15.16, contiguo en ci dentro de co)
 *   Cout      : nº de canales de salida
 *   bias      : puntero a bias[Cout] (Q?.32, escala del acumulador) o NULL
 *   shift_out : nº de bits a desplazar el acumulador (= 16 para Q15.16)
 *   y         : puntero a y[L][Cout]  (Q15.16, saturado a int32)
 */
void conv1d_k1_q1516(const int32_t *x, int L, int Cin,
                     const int32_t *W, int Cout,
                     const int64_t *bias,
                     int shift_out,
                     int32_t *y);

#ifdef __cplusplus
}
#endif

#endif /* CONV1D_Q1516_H */
