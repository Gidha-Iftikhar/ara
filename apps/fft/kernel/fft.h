#ifndef _FFTLIB_H
#define _FFTLIB_H

typedef signed short v2s __attribute__((vector_size (4)));
typedef float v2f __attribute__((vector_size (8)));
typedef __fp16 v2sf __attribute__((vector_size (4)));

// Default data type
#ifndef dtype
#define dtype float
#endif

// Define the corresponding vector type
#define vtype v2f


static inline v2s cplxmuls(v2s x, v2s y);
static inline vtype cplxmuls_float(vtype x, vtype y);
static inline v2s cplxmulsdiv2(v2s x, v2s y);
void  __attribute__ ((__noinline__)) SetupInput(signed short *In, int N, int Dyn);
void SetupR2SwapTable (short int *SwapTable, int Ni);
void SetupTwiddlesLUT(signed short *Twiddles, int Nfft, int Inverse);

void Radix4FFT_DIT_Scalar(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_FFT4, unsigned int Inverse);
void Radix4FFT_DIF_Scalar(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_FFT4, unsigned int Inverse);
void Radix2FFT_DIT_Scalar(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_FFT2);
void Radix2FFT_DIF_Scalar(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_FFT2);

void Radix4FFT_DIT(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_FFT4, unsigned int Inverse);
void Radix4FFT_DIF(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_FFT4, unsigned int Inverse);

void Radix2FFT_DIT(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_FFT2);
void Radix2FFT_DIF(signed short *__restrict__ Data, signed short *__restrict__ Twiddles, int N_FFT2);
void Radix2FFT_DIT_float(dtype *__restrict__ Data, dtype *__restrict__ Twiddles, int N_FFT2);
void Radix2FFT_DIF_float(dtype *__restrict__ Data, dtype *__restrict__ Twiddles, int N_FFT2);

void SwapSamples (vtype *__restrict__ Data, short *__restrict__ SwapTable, int Ni);

void cmplx2reim(v2f* cmplx);

#ifdef BUILD_LUT
void SetupTwiddlesLUT(signed short *Twiddles, int Nfft, int Inverse);
void SetupScalarTwiddlesLUT(signed short *Twiddles, int Nfft, int Inverse);

void SetupR2SwapTable (short int *SwapTable, int Ni);
#endif

#define FFT4_SAMPLE_DYN 12
#define FFT2_SAMPLE_DYN 13
#define FFT_TWIDDLE_DYN 15

#define FFT4_SCALEDOWN 2
#define FFT2_SCALEDOWN 1

#endif
