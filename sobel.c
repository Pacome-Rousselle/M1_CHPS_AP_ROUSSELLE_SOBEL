/*
  This code performs edge detection using a Sobel filter on a video stream meant as input to a neural network
*/

//
#include "kernels.h"

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

// 
int run_benchmark(const i8 *title,
		   void (*kernel)(u8 *, u8 *, f32), 
       i8 **paths);

//
int main(int argc, char **argv)
{
  //
  if (argc < 3)
    return printf("Usage: %s [raw input file] [raw output file]\n", argv[0]), 1;

  fprintf(stdout, "%10s; %10s; %15s; %15s; %15s; %10s; %26s;\n",
  "Kernel",
  "nb bytes",
  "min ns",
  "max ns",
  "mean ns",
  "MiB/s",
  "stddev(%)");

  run_benchmark("BASE", sobel_baseline,argv);
  run_benchmark("UNROLL3", sobel_unroll, argv);
  run_benchmark("NOZERO + UNROLL", sobel_maths,argv);
  run_benchmark("OMP", sobel_omp, argv);
  run_benchmark("NOSQRT", sobel_square,argv);
  run_benchmark("ALLOPTS", sobel_allopts,argv);
  return  0;
}

int run_benchmark(const i8 *title,
		   void (*kernel)(u8 *, u8 *, f32), 
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
  fprintf(stdout, "%10s; %10llu; %15.3lf; %15.3lf; %15.3lf; %15.3lf; %15.3lf;\n",
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
  
  return 0;
}