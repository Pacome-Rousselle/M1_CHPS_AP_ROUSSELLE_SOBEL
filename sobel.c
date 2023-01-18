/*
  This code performs edge detection using a Sobel filter on a video stream meant as input to a neural network
*/
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <emmintrin.h>
#include <cblas.h>
#include <clapack.h>
#include <lapacke.h>

//
#include "common.h"

//Convert an image to its grayscale equivalent - better color precision
void grayscale_weighted(u8 *frame)
{
  f32 gray;
  
  for (u64 i = 0; i < H * W * 3; i += 3)
    {
      //Convert RGB color values into grayscale for each pixel using color weights
      //Other possible weights: 0.59, 0.30, 0.11
      //Principle: mix different quantities of R, G, B to create a variant of gray
      gray = ((float)frame[i] * 0.299) + ((float)frame[i + 1] * 0.587) + ((float)frame[i + 2] * 0.114);
      
      frame[i]     = gray;
      frame[i + 1] = gray;
      frame[i + 2] = gray;
    }
}

//Convert an image to its grayscale equivalent - bad color precision
void grayscale_sampled(u8 *frame)
{
  for (u64 i = 0; i < H * W * 3; i += 3)
    {
      //R: light gray
      //G: medium gray
      //B: dark gray
      u8 gray = frame[i];
      
      frame[i]     = gray;
      frame[i + 1] = gray;
      frame[i + 2] = gray;
    }
}

/* Kernels */
// Baseline
i32 convolve_baseline(u8 *m, i32 *f, u64 fh, u64 fw)
{
  i32 r = 0;

  for (u64 i = 0; i < fh; i++)
    for (u64 j = 0; j < fw; j++)
      r += m[INDEX(i, j * 3, W * 3)] * f[INDEX(i, j, fw)];
  // Unrolling 4 and 8 times
  // Try OMP on the first loop
  // assembly (try SSE, AVX, check if AVX 512 available (must be a #define flag for that))
  // check for intrinsincs
  return r;
}

//
void sobel_baseline(u8 *restrict cframe, u8 *restrict oframe, f32 threshold)
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
        //drnm2 or srnm2 on a 2-element vector could be a good replacement (gradient norm)
        
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
void sobel_unroll(u8 *restrict cframe, u8 *restrict oframe, f32 threshold)
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
f32 convolve_blas(u8 *m, f32 *f, u64 fh, u64 fw)
{
  f32 r = 0;

  for (u64 i = 0; i < fh; i++)
    for (u64 j = 0; j < fw; j++)
      r += m[INDEX(i, j * 3, W * 3)] * f[INDEX(i, j, fw)];
  return r;
}

void sobel_blas(u8 *restrict cframe, u8 *restrict oframe, f32 threshold)
{
  f32 *gradient = _mm_malloc(2*sizeof(f32),32);
  f32 mag = 0.0;

  f32 f1[9] = { -1.0, 0.0, 1.0,
		-2.0, 0.0, 2.0,
		-1.0, 0.0, 1.0 }; //3x3 matrix
  
  f32 f2[9] = { -1.0, -2.0, -1.0,
		0.0, 0.0, 0.0,
		1.0, 2.0, 1.0 }; //3x3 matrix
  
  //
  for (u64 i = 0; i < (H - 3); i++)
    for (u64 j = 0; j < ((W * 3) - 3); j++)
      {
        gradient[0] = convolve_blas(&cframe[INDEX(i, j, W * 3)], f1, 3, 3);
        gradient[1] = convolve_blas(&cframe[INDEX(i, j, W * 3)], f2, 3, 3);
            
        mag = cblas_snrm2(2,gradient,1);
        
        oframe[INDEX(i, j, W * 3)] = (mag > threshold) ? 255 : mag;
      }
}
// Vectorization


// 
int run_benchmark(const i8 *title,
		   void (*kernel)(u8 *restrict, u8 *restrict, f32), 
       i8 **paths);

//
int main(int argc, char **argv)
{
  //
  if (argc < 3)
    return printf("Usage: %s [raw input file] [raw output file]\n", argv[0]), 1;
  
  run_benchmark("RESTRICT", sobel_baseline,argv);
  run_benchmark("UNROLL3", sobel_unroll, argv);
  run_benchmark("BLAS", sobel_blas,argv);
  

  return  0;
}

int run_benchmark(const i8 *title,
		   void (*kernel)(u8 *restrict, u8 *restrict, f32), 
       i8 **paths)
{ 
  //Try bufferizing I/O here
  
  //Size of a frame
  u64 size = sizeof(u8) * H * W * 3;

  //
  f64 elapsed_s = 0.0;
  f64 elapsed_ns = 0.0;
  f64 mib_per_s = 0.0;
  struct timespec t1, t2;
  f64 samples[MAX_SAMPLES];
  //
  u64 nb_bytes = 1, frame_count = 0, samples_count = 0;
  
  //
  u8 *cframe = _mm_malloc(size, 32);
  u8 *oframe = _mm_malloc(size, 32);

  //
  FILE *fpi = fopen(paths[1], "rb"); 
  FILE *fpo = fopen(paths[2], "wb");

  //
  if (!fpi)
    return printf("Error: cannot open file '%s'\n", paths[1]), 2;
  
  //
  if (!fpo)
    return printf("Error: cannot open file '%s'\n", paths[2]), 2;
  
  //Read & process video frames
  while ((nb_bytes = fread(cframe, sizeof(u8), H * W * 3, fpi)))
  {
    //
    grayscale_weighted(cframe);

    do
    {
        
      //Start 
      clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
      
      //Kernel
      kernel(cframe, oframe, 100.0);
      //Stop
      clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
        
      //Nano seconds
      elapsed_ns = (f64)(t2.tv_nsec - t1.tv_nsec);
        
    }
    while (elapsed_ns <= 0.0);
      
    //Seconds
    elapsed_s = elapsed_ns / 1e9;
      
    //2 arrays
    mib_per_s = ((f64)(nb_bytes << 1) / (1024.0 * 1024.0)) / elapsed_s;
      
    //
    if (samples_count < MAX_SAMPLES)
	    samples[samples_count++] = elapsed_ns;
      
    //frame number; size in Bytes; elapsed ns; bytes per second
    //fprintf(stdout, "%20llu; %20llu bytes; %15.3lf ns; %15.3lf MiB/s\n", frame_count, nb_bytes << 1, elapsed_ns, mib_per_s);
      
    // Write this frame to the output pipe
    fwrite(oframe, sizeof(u8), H * W * 3, fpo);

    //
    frame_count++;
  }

  //
  sort(samples, samples_count);

  //
  f64 min, max, avg, mea, dev;

  //
  mea = mean(samples, samples_count);
  
  //
  dev = stddev(samples, samples_count);

  //
  min = samples[0];
  max = samples[samples_count - 1];
  
  elapsed_s = mea / 1e9;

  //2 arrays (input & output)
  mib_per_s = ((f64)(size << 1) / (1024.0 * 1024.0)) / elapsed_s;
  
  // bytes, min ns, max ns, avg ns, MiB/s, stddev
  FILE *dat = fopen("dat/plot.dat", "a");
  if (!dat)
    return printf("Error: cannot open file 'plot.dat'\n"), 2;
  fprintf(dat, "%10s; %10llu; %15.3lf; %15.3lf; %15.3lf; %15.3lf; %15.3lf;\n",
    title,
	  (u64)(sizeof(u8) * H * W * 3) << 1,
	  min,
	  max,
	  mea,
	  mib_per_s,
	  (dev * 100.0 / mea));
  
  //
  _mm_free(cframe);
  _mm_free(oframe);

  //
  fclose(fpi);
  fclose(fpo);
  fclose(dat);
}