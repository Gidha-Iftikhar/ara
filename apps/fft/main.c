// Copyright 2020 ETH Zurich and University of Bologna.
//
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Author: Matteo Perotti <mperotti@iis.ee.ethz.ch>

#include <stdint.h>
#include <string.h>

#include "kernel/fft.h"
#include "printf.h"
#include "runtime.h"

#define MAX_NFFT 4096

extern unsigned long int NFFT;

extern dtype twiddle[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern dtype twiddle_v[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern dtype samples[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
dtype samples_copy[MAX_NFFT] __attribute__((aligned(32 * NR_LANES), section(".l2")));
dtype samples_vec[MAX_NFFT] __attribute__((aligned(32 * NR_LANES), section(".l2")));
dtype buf[MAX_NFFT] __attribute__((aligned(32 * NR_LANES), section(".l2")));
extern dtype gold_out[] __attribute__((aligned(32 * NR_LANES), section(".l2")));
signed short SwapTable[MAX_NFFT] __attribute__((aligned(32 * NR_LANES), section(".l2")));

int main() {
  printf("\n");
  printf("=========\n");
  printf("=  FFT  =\n");
  printf("=========\n");
  printf("\n");
  printf("\n");

  printf("\n");
  printf("------------------------------------------------------------\n");
  printf("------------------------------------------------------------\n");
  printf("\n");

  printf("Radix 2 DIF FFT on %d points\n", NFFT);

  ////////////////////
  // INITIALIZATION //
  ////////////////////

  // Initialize Twiddle factors
  printf("Initializing Twiddle Factors\n");
  //SetupTwiddlesLUT(twiddle, NFFT, 0);
  // Initialize Swap Table
  printf("Initializing Swap Table\n");
  SetupR2SwapTable(SwapTable, NFFT);
  // Initialize Inputs
  printf("Initializing Inputs for DIT\n");
  //SetupInput(In_DIT, NFFT, FFT2_SAMPLE_DYN);
  memcpy((void*) samples_copy, (void*) samples, 2 * NFFT * sizeof(dtype));
  memcpy((void*) samples_vec, (void*) samples, 2 * NFFT * sizeof(dtype));

  // Print the input
  for (int i = 0; i < 2*NFFT; ++i) {
    if (!(i % 2)) printf("In_DIT[%d] == %f ", i / 2, samples[i]);
    else          printf("+ (%f)j\n", samples[i]);
  }
  printf("\n");
  for (int i = 0; i < 2*NFFT; ++i) {
    if (!(i % 2)) printf("In_DIF[%d] == %f ", i / 2, samples_copy[i]);
    else          printf("+ (%f)j\n", samples_copy[i]);
  }

  // Swap samples for DIT FFT
  SwapSamples((vtype *) samples, SwapTable, NFFT);

  /////////////
  // DIT FFT //
  /////////////

  start_timer();
  Radix2FFT_DIT_float(samples, twiddle, NFFT);
  stop_timer();

  // Performance metrics
  int64_t runtime = get_timer();
  printf("The DIT execution took %d cycles.\n", runtime);

  /////////////
  // DIF FFT //
  /////////////

  start_timer();
  Radix2FFT_DIF_float(samples_copy, twiddle, NFFT);
  stop_timer();
  SwapSamples((vtype *) samples_copy, SwapTable, NFFT);

  printf("Initializing Inputs for DIF\n");

  // Performance metrics
  runtime = get_timer();
  printf("The DIF execution took %d cycles.\n", runtime);

  ////////////////////
  // Vector DIF FFT //
  ////////////////////

  float* samples_reim = cmplx2reim(samples_vec, buf, NFFT);
  fft_r2dif_vec(samples_reim, samples_reim + NFFT,
                   const float* twiddles_re, const float* twiddles_im, NFFT);

/*
  // Sanity check
  printf("In_DIT[0] = %d\n", samples[0]);
  printf("In_DIF[0] = %d\n", samples_copy[0]);
  printf("gold_out[0] = %d\n", gold_out[0]);
*/

  // Print the results
  for (int i = 0; i < 2*NFFT; ++i) {
    if (!(i % 2)) printf("In_DIT[%d] == %f ", i / 2, samples[i]);
    else          printf("+ (%f)j\n", samples[i]);
  }
  printf("\n");
  for (int i = 0; i < 2*NFFT; ++i) {
    if (!(i % 2)) printf("In_DIF[%d] == %f ", i / 2, samples_copy[i]);
    else          printf("+ (%f)j\n", samples_copy[i]);
  }
  printf("\n");
  for (int i = 0; i < 2*NFFT; ++i) {
    if (!(i % 2)) printf("gold_out[%d] == %f ", i / 2, gold_out[i]);
    else          printf("+ (%f)j\n", gold_out[i]);
  }
/*
  // Check
  for (int i = 0; i < NFFT; ++i) {
    if (samples[i] != samples_copy[i]) {
      printf("In_DIT[%d] != In_DIF[%d]\n", i, i);
      return 1;
    }
  }
  // Check
  for (int i = 0; i < NFFT; ++i) {
    if (samples[i] != gold_out[i]) {
      printf("In_DIT[%d] != gold_out[%d]\n", i, i);
      return 1;
    }
  }
*/

  return 0;
}
