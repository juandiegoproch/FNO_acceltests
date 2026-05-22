/* conv1d.c - implementaciones bare-metal, sin malloc.
 *
 * Layout: x[L][Cin]  (channels-last, HWC reinterpretado como LC).
 *         y[Lout][Cout]
 *         W[Cout][Cin][K]  (igual que PyTorch)
 *
 * Para [n][n][24] reinterpretado como [n*n][24], cada posición espacial
 * tiene los Cin canales contiguos en memoria. Esto convierte el inner loop
 * sobre canales en un dot-product sobre memoria contigua: ideal para
 * vectorización automática del compilador y prefetch.
 */
#include "conv1d.h"

/* ---------- Helpers de saturación para fixed-point ----------------------- */
static inline int16_t sat_i16(int32_t v) {
    if (v >  32767) return  32767;
    if (v < -32768) return -32768;
    return (int16_t)v;
}
static inline int8_t sat_i8(int32_t v) {
    if (v >  127) return  127;
    if (v < -128) return -128;
    return (int8_t)v;
}

/* Rounding shift right: equivalente a CMSIS-NN __SSAT(round(x * 2^-n)).
 * Suma 0.5 ULP antes de desplazar para redondeo correcto al más cercano. */
static inline int32_t rshift_round(int32_t v, int n) {
    if (n <= 0) return v;
    int32_t bias = (int32_t)1 << (n - 1);
    /* Cuidado con overflow: v + bias podría desbordar. En práctica el
     * acumulador ya es int32 y los productos quedan dentro del rango. */
    return (v + bias) >> n;
}

/* ===========================================================================
 *  Macro-template: el mismo cuerpo se instancia para f32 / q15 / q7.
 *  Esto es exactamente el patrón que CMSIS-DSP usa internamente.
 * =========================================================================== */

/* ---------- Float32: caso general ---------------------------------------- */
void conv1d_f32(const float *x, int L, int Cin,
                const float *W, int Cout, int K,
                const float *bias,
                int stride, int pad, int dilation,
                float *y)
{
    const int Lout = conv1d_out_len(L, K, stride, pad, dilation);

    for (int lo = 0; lo < Lout; ++lo) {
        const int l_base = lo * stride - pad;
        float *yrow = &y[(size_t)lo * Cout];

        for (int co = 0; co < Cout; ++co) {
            float acc = bias ? bias[co] : 0.0f;
            const float *Wco = &W[(size_t)co * Cin * K];

            for (int k = 0; k < K; ++k) {
                const int li = l_base + k * dilation;
                if ((unsigned)li >= (unsigned)L) continue;   /* zero-pad */

                const float *xrow = &x[(size_t)li * Cin];    /* contiguo en memoria! */
                const float *Wk   = &Wco[(size_t)0 * K + k]; /* W[co][0][k] */

                /* dot-product sobre Cin con W[co][:][k]. Cin típico = 24,
                 * el compilador puede unroll/vectorizar este loop. */
                for (int ci = 0; ci < Cin; ++ci) {
                    acc += xrow[ci] * Wk[ci * K];
                }
            }
            yrow[co] = acc;
        }
    }
}

/* ---------- Float32: K=1 (pointwise) -------------------------------------
 * Es matemáticamente Y = X · W^T + b, donde X es (L,Cin), W es (Cout,Cin),
 * Y es (L,Cout). Como Cin es contiguo en x[l][:] y en W[co][:], cada
 * salida es un dot-product de Cin elementos contiguos: cache-óptimo.
 */
void conv1d_k1_f32(const float *x, int L, int Cin,
                   const float *W, int Cout,
                   const float *bias,
                   float *y)
{
    for (int l = 0; l < L; ++l) {
        const float *xrow = &x[(size_t)l * Cin];
        float       *yrow = &y[(size_t)l * Cout];

        for (int co = 0; co < Cout; ++co) {
            const float *Wco = &W[(size_t)co * Cin];
            float acc = bias ? bias[co] : 0.0f;

            /* Hint al compilador: con Cin=24 y -O3, GCC/Clang vectorizan
             * a NEON/SSE/AVX automáticamente. */
            #pragma GCC ivdep
            for (int ci = 0; ci < Cin; ++ci) {
                acc += xrow[ci] * Wco[ci];
            }
            yrow[co] = acc;
        }
    }
}

/* ---------- Q15 (int16): caso general ------------------------------------ */
void conv1d_q15(const int16_t *x, int L, int Cin,
                const int16_t *W, int Cout, int K,
                const int32_t *bias,
                int stride, int pad, int dilation,
                int shift_out,
                int16_t *y)
{
    const int Lout = conv1d_out_len(L, K, stride, pad, dilation);

    for (int lo = 0; lo < Lout; ++lo) {
        const int l_base = lo * stride - pad;
        int16_t *yrow = &y[(size_t)lo * Cout];

        for (int co = 0; co < Cout; ++co) {
            /* Acumulador int64 para máxima seguridad ante Cin*K grandes.
             * Para Cin*K <= 2^17 con valores Q1.15, int32 también basta. */
            int64_t acc = bias ? (int64_t)bias[co] : 0;
            const int16_t *Wco = &W[(size_t)co * Cin * K];

            for (int k = 0; k < K; ++k) {
                const int li = l_base + k * dilation;
                if ((unsigned)li >= (unsigned)L) continue;

                const int16_t *xrow = &x[(size_t)li * Cin];

                for (int ci = 0; ci < Cin; ++ci) {
                    /* (int32_t) cast asegura multiplicación de 32 bits en arquitecturas
                     * donde int es 16 bits (raro hoy, pero portable). */
                    acc += (int32_t)xrow[ci] * (int32_t)Wco[ci * K + k];
                }
            }
            yrow[co] = sat_i16((int32_t)rshift_round((int32_t)acc, shift_out));
        }
    }
}

/* ---------- Q15: K=1 ------------------------------------------------------ */
void conv1d_k1_q15(const int16_t *x, int L, int Cin,
                   const int16_t *W, int Cout,
                   const int32_t *bias,
                   int shift_out,
                   int16_t *y)
{
    for (int l = 0; l < L; ++l) {
        const int16_t *xrow = &x[(size_t)l * Cin];
        int16_t       *yrow = &y[(size_t)l * Cout];

        for (int co = 0; co < Cout; ++co) {
            const int16_t *Wco = &W[(size_t)co * Cin];
            int32_t acc = bias ? bias[co] : 0;

            for (int ci = 0; ci < Cin; ++ci) {
                acc += (int32_t)xrow[ci] * (int32_t)Wco[ci];
            }
            yrow[co] = sat_i16(rshift_round(acc, shift_out));
        }
    }
}

/* ---------- Q7 (int8): caso general -------------------------------------- */
void conv1d_q7(const int8_t *x, int L, int Cin,
               const int8_t *W, int Cout, int K,
               const int32_t *bias,
               int stride, int pad, int dilation,
               int shift_out,
               int8_t *y)
{
    const int Lout = conv1d_out_len(L, K, stride, pad, dilation);

    for (int lo = 0; lo < Lout; ++lo) {
        const int l_base = lo * stride - pad;
        int8_t *yrow = &y[(size_t)lo * Cout];

        for (int co = 0; co < Cout; ++co) {
            int32_t acc = bias ? bias[co] : 0;
            const int8_t *Wco = &W[(size_t)co * Cin * K];

            for (int k = 0; k < K; ++k) {
                const int li = l_base + k * dilation;
                if ((unsigned)li >= (unsigned)L) continue;

                const int8_t *xrow = &x[(size_t)li * Cin];

                for (int ci = 0; ci < Cin; ++ci) {
                    acc += (int32_t)xrow[ci] * (int32_t)Wco[ci * K + k];
                }
            }
            yrow[co] = sat_i8(rshift_round(acc, shift_out));
        }
    }
}

/* ---------- Q7: K=1 ------------------------------------------------------- */
void conv1d_k1_q7(const int8_t *x, int L, int Cin,
                  const int8_t *W, int Cout,
                  const int32_t *bias,
                  int shift_out,
                  int8_t *y)
{
    for (int l = 0; l < L; ++l) {
        const int8_t *xrow = &x[(size_t)l * Cin];
        int8_t       *yrow = &y[(size_t)l * Cout];

        for (int co = 0; co < Cout; ++co) {
            const int8_t *Wco = &W[(size_t)co * Cin];
            int32_t acc = bias ? bias[co] : 0;

            for (int ci = 0; ci < Cin; ++ci) {
                acc += (int32_t)xrow[ci] * (int32_t)Wco[ci];
            }
            yrow[co] = sat_i8(rshift_round(acc, shift_out));
        }
    }
}
