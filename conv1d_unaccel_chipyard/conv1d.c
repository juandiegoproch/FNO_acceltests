#include "conv1d.h"

static inline int32_t sat_i32(int64_t v) {
    if (v >  (int64_t)INT32_MAX) return INT32_MAX;
    if (v <  (int64_t)INT32_MIN) return INT32_MIN;
    return (int32_t)v;
}

static inline int64_t rshift_round_64(int64_t v, int n) {
    if (n <= 0) return v;
    int64_t bias = (int64_t)1 << (n - 1);
    return (v + bias) >> n;
}

void conv1d_k1_q1516(const int32_t *x, int L, int Cin,
                     const int32_t *W, int Cout,
                     const int64_t *bias,
                     int shift_out,
                     int32_t *y)
{
    for (int l = 0; l < L; ++l) {
        const int32_t *xrow = &x[(size_t)l * Cin];
        int32_t       *yrow = &y[(size_t)l * Cout];

        for (int co = 0; co < Cout; ++co) {
            int64_t acc = bias ? bias[co] : (int64_t)0;
            const int32_t *Wco = &W[(size_t)co * Cin]; 
            for (int ci = 0; ci < Cin; ++ci) {
                acc += (int64_t)xrow[ci] * (int64_t)Wco[ci];
            }
            yrow[co] = sat_i32(rshift_round_64(acc, shift_out));
        }
    }
}
