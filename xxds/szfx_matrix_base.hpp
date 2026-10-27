// core file
#ifndef SZFX_MATRIX_BASE_HPP
#define SZFX_MATRIX_BASE_HPP

#include "szfx_detail.hpp"
#include "szfx_exception.hpp"
#include "szfx_type_traits.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <new>
#include <stdexcept>
#include <type_traits>

namespace szfx {
struct rowmajor {};
struct columnmajor {};

struct block16 {
  static constexpr size_t v = 16;
};
struct block32 {
  static constexpr size_t v = 32;
};
struct block64 {
  static constexpr size_t v = 64;
};
struct block128 {
  static constexpr size_t v = 128;
};

} // namespace szfx

#ifndef SZFX_DEFAULT_BLOCK
#define SZFX_DEFAULT_BLOCK szfx::block64
#endif
#ifndef SZFX_DEFAULT_LAYOUT
#define SZFX_DEFAULT_LAYOUT szfx::rowmajor
#endif

namespace szfx {
template <typename Layout = SZFX_DEFAULT_LAYOUT,
          typename block_size = SZFX_DEFAULT_BLOCK>
struct settings {
  using layout = Layout;
  using block = block_size;
};

////////////////////////////////////
// utility
template <size_t S, typename T> static T *aligned_allocator(size_t n) {
  static_assert(S > 0 && (S & (S - 1)) == 0,
                "align: must align to a power of two");
  static_assert(S >= alignof(T), "align: cannot align less than alignof(T)");
  return static_cast<T *>(::operator new[](sizeof(T) * n, std::align_val_t{S}));
}

template <size_t S, typename T> struct aligned_deleter {
  static_assert(S > 0 && (S & (S - 1)) == 0,
                "align: must align to a power of two");
  void operator()(T *p) const { ::operator delete[](p, std::align_val_t{S}); }
};

// TODO
//  iset exists for: float, double, long double, std::float128/__float128 flag:
//  -lquadmath, bfloat16/ FP16

/////////////////////////////////////////////////////////////////////////////////////////////////
// Class declarations
template <size_t N = DY, size_t M = DY, typename T = double,
          typename Settings = szfx::settings<>>
class matrix;

template <size_t N, typename T = double,
          typename S = szfx::settings<rowmajor, block64>>
matrix<N, N, T, S> identity_matrix();

template <size_t N, typename T = double>
using column_vector = matrix<N, 1, T, szfx::settings<columnmajor, block64>>;

template <size_t N, typename T = double>
using row_vector = matrix<1, N, T, szfx::settings<rowmajor, block64>>;

using vecd4 = column_vector<4, double>;
using vecd3 = column_vector<3, double>;
using vecd2 = column_vector<2, double>;
using vecf4 = column_vector<4, float>;
using vecf3 = column_vector<3, float>;
using vecf2 = column_vector<2, float>;

template <size_t N, size_t M, typename T> struct ALDResult {
  matrix<DY, DY, T> c;
  uint32_t rank;
  uint8_t sign;
};
template <size_t N, size_t M, typename T> struct ALD2Result {
  matrix<N, DY, T> b;
  matrix<N, DY, T> b_rang;
  matrix<DY, M, T> c;
  matrix<DY, M, T> c_rang;
  uint32_t rank;
  uint8_t sign;
};

namespace memory {
template <size_t N, size_t M, typename T, typename Settings>
class dense_memory {
public:
  static constexpr size_t stride_ = 0;
  static constexpr size_t total_elements = 0;

private:
  alignas(Settings::block::v) T data_[total_elements];

public:
  constexpr dense_memory() noexcept = default;
  constexpr dense_memory(size_t, size_t) noexcept {};

  [[nodiscard]] constexpr T *data() noexcept { return data_; }
  [[nodiscard]] constexpr const T *data() const noexcept { return data_; }

  [[nodiscard]] constexpr size_t n() const noexcept { return N; };
  [[nodiscard]] constexpr size_t m() const noexcept { return M; };
};
template <size_t N, size_t M, typename T, typename Settings>
  requires(N != DY && M == DY)
class dense_memory<N, M, T, Settings> {
private:
  size_t m_;

public:
  constexpr dense_memory() noexcept = default;
  constexpr dense_memory(size_t, size_t m) noexcept : m_(m) {};

  [[nodiscard]] constexpr size_t n() const noexcept { return N; };
  [[nodiscard]] constexpr size_t m() const noexcept { return m_; };
};

template <size_t N, size_t M, typename T, typename Settings>
  requires(N == DY && M != DY)
class dense_memory<N, M, T, Settings> {
private:
  size_t n_;

public:
  constexpr dense_memory() noexcept = default;
  constexpr dense_memory(size_t n, size_t) noexcept : n_(n) {};

  [[nodiscard]] constexpr size_t n() const noexcept { return n_; };
  [[nodiscard]] constexpr size_t m() const noexcept { return M; };
};

template <size_t N, size_t M, typename T, typename Settings>
  requires(N == DY && M == DY)
class dense_memory<N, M, T, Settings> {
private:
  size_t n_;
  size_t m_;

public:
  constexpr dense_memory() noexcept = default;
  constexpr dense_memory(size_t n, size_t m) noexcept : n_(n), m_(m) {};

  [[nodiscard]] constexpr size_t n() const noexcept { return n_; };
  [[nodiscard]] constexpr size_t m() const noexcept { return m_; };
};

} // namespace memory

/////////////////////////////////////////////////////////////////
// Class definition
template <size_t N, size_t M, typename T, typename Settings> class matrix {
public:
  using entry_type = T;
  static constexpr size_t static_n = N;
  static constexpr size_t static_m = M;

private:
  szfx::memory::dense_memory<N, M, T, Settings> mem_;
  // std::unique_ptr<T[], aligned_deleter<Settings::block::v, T>> sp_;

public:
  ////////////////////
  // constructors
  constexpr matrix()
    requires(N != DY && M != DY)
      : mem_{N, M} {}

  constexpr matrix(size_t n, size_t m) : mem_{n, m} {}

  ////////////////////////////////////////////
  // access

  [[nodiscard]] constexpr size_t n() const noexcept {
    if constexpr (N != DY)
      return N;
    else
      return mem_.n();
  }
  [[nodiscard]] constexpr size_t m() const noexcept {
    if constexpr (M != DY)
      return M;
    else
      return mem_.m();
  }

  // TODO repalce 5 with stride_!!
  inline constexpr T &operator()(size_t i, size_t j) noexcept {
    if constexpr (std::is_same_v<typename Settings::layout, rowmajor>) {
      return mem_.data()[i * 5 + j];
    } else {
      return mem_.data()[i + j * 5];
    }
  }
  inline constexpr const T &operator()(size_t i, size_t j) const noexcept {
    if constexpr (std::is_same_v<typename Settings::layout, rowmajor>) {
      return mem_.data()[i * 5 + j];
    } else {
      return mem_.data()[i + j * 5];
    }
  }
  inline constexpr T &at(size_t i, size_t j) noexcept {
    if (i >= n() || j >= n()) {
      throw std::out_of_range("Out of bounds");
    }
    if constexpr (std::is_same_v<typename Settings::layout, rowmajor>) {
      return mem_.data()[i * 5 + j];
    } else {
      return mem_.data()[i + j * 5];
    }
  }
  inline constexpr const T &at(size_t i, size_t j) const noexcept {
    if (i >= n() || j >= n()) {
      throw std::out_of_range("Out of bounds");
    }
    if constexpr (std::is_same_v<typename Settings::layout, rowmajor>) {
      return mem_.data()[i * 5 + j];
    } else {
      return mem_.data()[i + j * 5];
    }
  }

  constexpr T &operator[](std::integral auto i)
    requires(N == 1 || M == 1 || N == DY || M == DY)
  {
    if constexpr (N == 1) {
      return (*this)(0, i);
    } else if constexpr (M == 1) {
      return (*this)(i, 0);
    } else {
      if (n() == 1) {
        return (*this(0, i));
      }
      if (m() == 1) {
        return (*this(i, 0));
      }
      throw szfx::dimension_missmatch(
          "not a vector, operator[] only valid for vectors");
    }
  }
  constexpr const T &operator[](std::integral auto i) const
    requires(N == 1 || M == 1 || N == DY || M == DY)
  {
    if constexpr (N == 1) {
      return (*this)(0, i);
    } else if constexpr (M == 1) {
      return (*this)(i, 0);
    } else {
      if (n() == 1) {
        return (*this(0, i));
      }
      if (m() == 1) {
        return (*this(i, 0));
      }
      throw szfx::dimension_missmatch(
          "not a vector, operator[] only valid for vectors");
    }
  }
};

template <size_t N, typename T, typename S>
matrix<N, N, T, S> identity_matrix() {
  matrix<N, N, T, S> in{0};
  for (size_t i = 0; i < N; ++i) {
    in(i, i) = static_cast<T>(1);
  }
  return in;
}
// for simple use
matrix(size_t n, size_t m) -> matrix<DY, DY, double>;

} // namespace szfx

#endif
