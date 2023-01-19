#include "common.h"
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>

/* Kernels */
i32 convolve_baseline(u8 *restrict m, i32 *restrict f, u64 fh, u64 fw);
void sobel_baseline(u8 *restrict cframe, u8 *restrict oframe, f32 threshold);

i32 convolve_unroll(u8 *restrict m, i32 *restrict f, u64 fh, u64 fw);
void sobel_unroll(u8 *restrict cframe, u8 *restrict oframe, f32 threshold);

i32 convolve_right(u8 *restrict m, i32 *restrict f, u64 fh, u64 fw);
i32 convolve_bottom(u8 *restrict m, i32 *restrict f, u64 fh, u64 fw);
void sobel_maths(u8 *restrict cframe, u8 *restrict oframe, f32 threshold);