#ifndef CONV1D_Q1516_H
#define CONV1D_Q1516_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void conv1d_k1_q1516(const int32_t *x, int L, int Cin,
                     const int32_t *W, int Cout,
                     const int64_t *bias,
                     int shift_out,
                     int32_t *y);

#ifdef __cplusplus
}
#endif

#endif /* CONV1D_Q1516_H */
