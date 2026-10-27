#ifndef SZFX_SIMD_HPP
#define SZFX_SIMD_HPP

#if defined(__x86_64__) || defined(_M_X64) ||                                  \
    ((defined(__i386__) || defined(_M_IX86)) &&                                \
     (defined(__SSE2__) || (defined(_M_IX86_FP) && _M_IX86_FP >= 2)))

#define SZFX_ARCH_X86_SSE2 1
#include <immintrin.h>

#if defined(__AVX2__)
#define SZFX_HAS_AVX2 1
#endif

#if defined(__AVX__)
#define SZFX_HAS_AVX 1
#endif

#if defined(__AVX512F__) || defined(__AVX512__)
#define SZFX_HAS_AVX512 1
#endif

#elif defined(__aarch64__) || defined(_M_ARM64) ||                             \
    ((defined(__arm__) || defined(_M_ARM)) &&                                  \
     (defined(__ARM_NEON) || defined(__ARM_NEON__) ||                          \
      (defined(_M_ARM_FP) && _M_ARM_FP >= 4)))
#define SZFX_ARCH_ARM_NEON 1
#include <arm_neon.h>

#elif defined(__wasm__) && defined(__wasm_simd128__)

#define SZFX_SIMD_WASM 1
#include <wasm_simd128.h>

#elif defined(__riscv) && defined(__riscv_vector)
#define SZFX_ARCH_RISCV 1
#include <riscv_vector.h>

#else
#define SZFX_ARCH_NO_SIMD 1

#endif

#endif
