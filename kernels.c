#include "kernels.h"

/* Kernels */
// Baseline
i32 convolve_baseline(u8 *m, i32 *f, u64 fh, u64 fw)
{
  i32 r = 0;

  for (u64 i = 0; i < fh; i++)
    for (u64 j = 0; j < fw; j++)
      r += m[INDEX(i, j * 3, W * 3)] * f[INDEX(i, j, fw)];
  
  return r;
}

//
void sobel_baseline(u8 *cframe, u8 *oframe, f32 threshold)
{
  i32 gx, gy;
  f32 mag = 0.0;

  i32 f1[9] = { -1, 0, 1,
		-2, 0, 2,
		-1, 0, 1 }; //3x3 matrix
  
  i32 f2[9] = { -1, -2, -1,
		0, 0, 0,
		1, 2, 1 }; //3x3 matrix
  
  //
  for (u64 i = 0; i < (H - 3); i++)
    for (u64 j = 0; j < ((W * 3) - 3); j++)
      {
	gx = convolve_baseline(&cframe[INDEX(i, j, W * 3)], f1, 3, 3);
	gy = convolve_baseline(&cframe[INDEX(i, j, W * 3)], f2, 3, 3);
      
	mag = sqrt((gx * gx) + (gy * gy));
	
	oframe[INDEX(i, j, W * 3)] = (mag > threshold) ? 255 : mag;
      }
}

// Unrolling the convolution
i32 convolve_unroll(u8 *m, i32 *f, u64 fh, u64 fw)
{
  i32 r = 0;

  for (u64 i = 0; i < fh; i++)
  {
    r += m[INDEX(i, 0, W * 3)] * f[INDEX(i, 0, fw)];
    r += m[INDEX(i, 3, W * 3)] * f[INDEX(i, 1, fw)];
    r += m[INDEX(i, 6, W * 3)] * f[INDEX(i, 2, fw)];  
  }
  return r;
}

//
void sobel_unroll(u8 *cframe, u8 *oframe, f32 threshold)
{
  i32 gx, gy;
  f32 mag = 0.0;

  i32 f1[9] = { -1, 0, 1,
		-2, 0, 2,
		-1, 0, 1 }; //3x3 matrix
  
  i32 f2[9] = { -1, -2, -1,
		0, 0, 0,
		1, 2, 1 }; //3x3 matrix
  
  //
  for (u64 i = 0; i < (H - 3); i++)
    for (u64 j = 0; j < ((W * 3) - 3); j++)
      {
        gx = convolve_unroll(&cframe[INDEX(i, j, W * 3)], f1, 3, 3);
        gy = convolve_unroll(&cframe[INDEX(i, j, W * 3)], f2, 3, 3);
            
        mag = sqrt((gx * gx) + (gy * gy));
        
        oframe[INDEX(i, j, W * 3)] = (mag > threshold) ? 255 : mag;
      }
}

// Blas
i32 convolve_right(u8 *m, i32 *f, u64 fh, u64 fw)
{
  i32 r = 0;

  for (u64 i = 0; i < fh; i++)
  {
    r += m[INDEX(i, 0, W * 3)] * f[INDEX(i, 0, fw)];
    r += m[INDEX(i, 6, W * 3)] * f[INDEX(i, 1, fw)]; 
  }
      
  return r;
}

i32 convolve_bottom(u8 *m, i32 *f, u64 fh, u64 fw)
{
  i32 r = 0;

  for (u64 i = 0; i < fh; i++)
  {
    r += m[INDEX(i, 0, W * 3 *(i*fh))] * f[INDEX(i, 0, fw)];
    r += m[INDEX(i, 3, W * 3*(i*fh))] * f[INDEX(i, 1, fw)];
    r += m[INDEX(i, 6, W * 3*(i*fh))] * f[INDEX(i, 2, fw)]; 
  }

  return r;
}

//
void sobel_maths(u8 *cframe, u8 *oframe, f32 threshold)
{
  i32 gx, gy;
  f32 mag = 0.0;

  i32 f1[6] = { -1, 1,
		-2, 2,
		-1, 1 }; //3x2 matrix
  
  i32 f2[6] = { -1, -2, -1,
		1, 2, 1 }; //2x3 matrix
  
  //
  for (u64 i = 0; i < (H - 3); i++)
    for (u64 j = 0; j < ((W * 3) - 3); j++)
      {
        gx = convolve_right(&cframe[INDEX(i, j, W * 3)], f1, 3, 2);
        gy = convolve_bottom(&cframe[INDEX(i, j, W * 3)], f2, 2, 3);
            
        mag = sqrt((gx * gx) + (gy * gy));
        
        oframe[INDEX(i, j, W * 3)] = (mag > threshold) ? 255 : mag;
      }
}
// Vectorization