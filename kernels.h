#include "common.h"
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>

/* Kernels */
i32 convolve_baseline(u8 *m, i32 *f, u64 fh, u64 fw);
void sobel_baseline(u8 *cframe, u8 *oframe, f32 threshold);

i32 convolve_unroll(u8 *m, i32 *f, u64 fh, u64 fw);
void sobel_unroll(u8 *cframe, u8 *oframe, f32 threshold);

i32 convolve_right(u8 *m, i32 *f, u64 fh, u64 fw);
i32 convolve_bottom(u8 *m, i32 *f, u64 fh, u64 fw);
void sobel_maths(u8 *a, u8 *b, f32 threshold);