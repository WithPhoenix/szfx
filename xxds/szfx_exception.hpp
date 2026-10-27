// new exceptions regarding dimension missmatch, static colliding with dynamic,
#ifndef SZFX_EXCEPTION_HPP
#define SZFX_EXCEPTION_HPP

#include <exception>
#include <string>

namespace szfx {
class dimension_missmatch : public std::exception {
private:
  std::string msg_;

public:
  dimension_missmatch(const char *msg) : msg_(msg) {};

  const char *what() const noexcept { return msg_.c_str(); }
};
class static_dynamic_dimension_exception : public std::exception {
private:
  std::string msg_;

public:
  static_dynamic_dimension_exception(const char *msg) : msg_(msg) {};

  const char *what() const noexcept { return msg_.c_str(); }
};

} // namespace szfx

#endif
