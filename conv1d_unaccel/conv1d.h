/* conv1d.h - bare-metal Conv1D for layout [L][Cin] (channels-last)
 *
 * Diseñado para un buffer físico x[n][n][24] reinterpretado como x[L=n*n][24].
 * Sin malloc, sin dependencias, equivalente a torch.nn.functional.conv1d
 * (cross-correlation, no flip), single sample (sin batch).
 *
 * Convención de pesos: W[Cout][Cin][K]  (igual que PyTorch)
 *   - PyTorch shape:     (Cout, Cin/groups, K)  con groups=1
 *   - Memoria row-major: w[co*Cin*K + ci*K + k]
 *
 * Convención de salida: y[Lout][Cout]  (channels-last, mismo layout que x)
 *
 * Lout = (L + 2*pad - dilation*(K-1) - 1) / stride + 1
 */
#ifndef CONV1D_H
#define CONV1D_H

#include <stdint.h>
#include <stddef.h>

/* --- Tipos fixed-point ----------------------------------------------------
 * q15: int16_t, formato Q1.15 (rango [-1, 1))
 * q7:  int8_t,  formato Q1.7  (rango [-1, 1))
 * Acumuladores en int32_t/int64_t para evitar overflow.
 * El usuario debe pasar shift_out: cuántos bits desplazar a la derecha
 * el acumulador final antes de saturar al tipo de salida.
 *   - Para Q15 con W,X en Q1.15: producto en Q2.30, shift_out típico = 15
 *   - Para Q7  con W,X en Q1.7:  producto en Q2.14, shift_out típico = 7
 */

#ifdef __cplusplus
extern "C" {
#endif

/* Cálculo de longitud de salida (idéntico a PyTorch). */
static inline int conv1d_out_len(int L, int K, int stride, int pad, int dil) {
    return (L + 2*pad - dil*(K-1) - 1) / stride + 1;
}

/* === Forward float32 ====================================================== */
void conv1d_f32(const float *x, int L, int Cin,           /* x[L][Cin]      */
                const float *W, int Cout, int K,          /* W[Cout][Cin][K]*/
                const float *bias,                        /* bias[Cout] o NULL */
                int stride, int pad, int dilation,
                float *y);                                /* y[Lout][Cout]  */

/* Caso especial K=1 (pointwise / channel-mixing GEMM). */
void conv1d_k1_f32(const float *x, int L, int Cin,        /* x[L][Cin]      */
                   const float *W, int Cout,              /* W[Cout][Cin]   */
                   const float *bias,                     /* bias[Cout] o NULL */
                   float *y);                             /* y[L][Cout]     */

/* === Forward int16 (Q15) ================================================== */
void conv1d_q15(const int16_t *x, int L, int Cin,
                const int16_t *W, int Cout, int K,
                const int32_t *bias,                      /* bias en Q-acumulador (int32) */
                int stride, int pad, int dilation,
                int shift_out,                            /* desplazamiento a la derecha */
                int16_t *y);

void conv1d_k1_q15(const int16_t *x, int L, int Cin,
                   const int16_t *W, int Cout,
                   const int32_t *bias,
                   int shift_out,
                   int16_t *y);

/* === Forward int8 (Q7) ==================================================== */
void conv1d_q7(const int8_t *x, int L, int Cin,
               const int8_t *W, int Cout, int K,
               const int32_t *bias,
               int stride, int pad, int dilation,
               int shift_out,
               int8_t *y);

void conv1d_k1_q7(const int8_t *x, int L, int Cin,
                  const int8_t *W, int Cout,
                  const int32_t *bias,
                  int shift_out,
                  int8_t *y);

/* === Dispatcher genérico C11 ============================================== */
#if (__STDC_VERSION__ >= 201112L) && !defined(CONV1D_NO_GENERIC)
/* Uso:
 *   conv1d(x, L, Cin, W, Cout, K, bias, s, p, d, y);
 * El tipo se infiere del puntero `x`. Para K=1 use conv1d_k1(...).
 */
#define conv1d(x, L, Cin, W, Cout, K, bias, s, p, d, y) \
    _Generic((x), \
        const float*:   conv1d_f32_g_,    float*:   conv1d_f32_g_,   \
        const int16_t*: conv1d_q15_g_,    int16_t*: conv1d_q15_g_,   \
        const int8_t*:  conv1d_q7_g_,     int8_t*:  conv1d_q7_g_     \
    )((x), (L), (Cin), (W), (Cout), (K), (bias), (s), (p), (d), (y))

/* Trampolines para que _Generic pueda despachar con el mismo arity:
 * los kernels q15/q7 toman shift_out = 15 / 7 por defecto vía el dispatcher.
 * Si necesitas otro shift, llama directamente a conv1d_q15 / conv1d_q7. */
static inline void conv1d_f32_g_(const float *x, int L, int Cin,
                                 const float *W, int Cout, int K,
                                 const float *bias, int s, int p, int d,
                                 float *y) {
    conv1d_f32(x, L, Cin, W, Cout, K, bias, s, p, d, y);
}
static inline void conv1d_q15_g_(const int16_t *x, int L, int Cin,
                                 const int16_t *W, int Cout, int K,
                                 const int32_t *bias, int s, int p, int d,
                                 int16_t *y) {
    conv1d_q15(x, L, Cin, W, Cout, K, bias, s, p, d, /*shift=*/15, y);
}
static inline void conv1d_q7_g_(const int8_t *x, int L, int Cin,
                                const int8_t *W, int Cout, int K,
                                const int32_t *bias, int s, int p, int d,
                                int8_t *y) {
    conv1d_q7(x, L, Cin, W, Cout, K, bias, s, p, d, /*shift=*/7, y);
}
#endif /* C11 _Generic */

#ifdef __cplusplus
}
#endif
#endif /* CONV1D_H */
