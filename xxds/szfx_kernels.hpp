// provides kernels that can be used in gemm,
// also offers simd_instruction set

#ifndef SZFXKERNEL_HPP
#define SZFXKERNEL_HPP

#include "szfx_simd.hpp"
#include <cstddef>
#include <type_traits>

namespace szfxkernel {

template <typename T> struct simd_instructionset {
  using t = T;
  static constexpr size_t width = 1;

  static inline t load(const t *p) { return *p; }
  static inline t set_zero() { return t(0); }
  static inline t fmadd(t a, T b, t c) { return a * b + c; }
  static inline t horizontal_add(t v) { return v; }
  template <typename I> static inline t load_from(const I *p) {
    return static_cast<t>(*p);
  }
};

template <> struct simd_instructionset<float> {
#if defined(SZFX_ARCH_X86_SSE2)
  using t = __m256;
  static constexpr size_t width = 8;
#elif defined(SZFX_ARCH_ARM_NEON)
#else
#endif
};

template <> struct simd_instructionset<double> {
#if defined(SZFX_ARCH_X86_SSE2)
  using t = __m256d;
  static constexpr size_t width = 4;
#elif defined(SZFX_ARCH_ARM_NEON)
  using t = float64x2_t;
  static constexpr size_t width = 2;
#else
#endif
};

////////////////////////////////////////////////////////////////////////////////////////////////
// register the instruction sets
template <typename T> struct has_iset : std::false_type {};
template <> struct has_iset<float> : std::true_type {};
template <> struct has_iset<double> : std::true_type {};
} // namespace szfxkernel

#endif
