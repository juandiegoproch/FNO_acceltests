/* conv1d.c — Implementación de la conv1d K=1 en Q15.16.
 *
 * Migrado desde ../conv1d_unaccel/conv1d.c:143-163 (`conv1d_k1_q15`,
 * int16/Q1.15, acumulador int32). Cambios únicos respecto al original:
 *   - Tipos x/W/y: int16_t → int32_t (16-bit Q1.15 → 32-bit Q15.16).
 *   - Bias: int32_t → int64_t (queda a la escala del acumulador, Q?.32).
 *   - Acumulador: int32_t → int64_t (porque el producto Q15.16×Q15.16 cabe
 *     en 64 bits, y acumular Cin=24 productos puede usar varios bits más).
 *   - Promoción de operandos: (int32_t)·(int32_t) → (int64_t)·(int64_t).
 *   - sat_i16 → sat_i32 (rango int32, no int16).
 *   - rshift_round (32-bit) → rshift_round_64 (64-bit) por el ancho de v.
 *   - shift_out típico pasa de 15 (Q1.15) a 16 (Q15.16). El parámetro sigue
 *     siendo configurable por el caller.
 *
 * Algoritmo, layout y orden de bucles SIN cambios: sigue siendo la
 * estructura GEMV (matrix-vector) por cada posición l ∈ [0, L), con la
 * misma matriz W reutilizada para todas las posiciones. Es exactamente lo
 * que un acelerador GEMV/MAC-array debería explotar.
 */

#include "conv1d.h"

/* --- Helpers de saturación y redondeo ------------------------------------ */

/* Satura un int64_t al rango representable en int32_t. */
static inline int32_t sat_i32(int64_t v) {
    if (v >  (int64_t)INT32_MAX) return INT32_MAX;
    if (v <  (int64_t)INT32_MIN) return INT32_MIN;
    return (int32_t)v;
}

/* Rounding shift right de 64 bits: equivalente a round(v * 2^-n).
 * Suma 0.5 ULP (bias = 1<<(n-1)) antes del shift para redondear al más
 * cercano. Si n <= 0, no desplaza. */
static inline int64_t rshift_round_64(int64_t v, int n) {
    if (n <= 0) return v;
    int64_t bias = (int64_t)1 << (n - 1);
    return (v + bias) >> n;
}

/* --- Conv1D K=1 en Q15.16 ------------------------------------------------- */

void conv1d_k1_q1516(const int32_t *x, int L, int Cin,
                     const int32_t *W, int Cout,
                     const int64_t *bias,
                     int shift_out,
                     int32_t *y)
{
    for (int l = 0; l < L; ++l) {
        const int32_t *xrow = &x[(size_t)l * Cin];   /* x[l][:] contiguo */
        int32_t       *yrow = &y[(size_t)l * Cout];

        for (int co = 0; co < Cout; ++co) {
            /* bias[co] vive ya a la escala del acumulador (Q?.32). */
            int64_t acc = bias ? bias[co] : (int64_t)0;
            const int32_t *Wco = &W[(size_t)co * Cin];  /* W[co][:] contiguo */

            /* Dot-product de Cin elementos. Cin típico = 24 ó 12, el
             * compilador puede unroll este bucle (con -O2 lo hace; con
             * -O0 — el flag heredado de fft_unaccel — no, lo que infla el
             * conteo de instrucciones pero hace la métrica reproducible). */
            for (int ci = 0; ci < Cin; ++ci) {
                acc += (int64_t)xrow[ci] * (int64_t)Wco[ci];
            }

            /* Q?.32 → Q15.16: shift derecha de 16 con redondeo y sat int32. */
            yrow[co] = sat_i32(rshift_round_64(acc, shift_out));
        }
    }
}
