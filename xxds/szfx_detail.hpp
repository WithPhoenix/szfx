// szfx internal functionality
#ifndef SZFX_DETAIL_HPP
#define SZFX_DETAIL_HPP

#include <cstddef>
#include <span>

namespace szfx {
inline static constexpr size_t DY = std::dynamic_extent;
}

namespace szfx::detail {

struct inner_flag {};
inline constexpr inner_flag using_inner{};
} // namespace szfx::detail

#endif
