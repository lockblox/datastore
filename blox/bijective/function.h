#pragma once
#include <functional>

namespace blox::bijective {

/** A bijective function f and its inverse g */
template <typename X, typename Y>
class function {
 public:
  using f_type = std::function<Y(const X&)>;
  using g_type = std::function<X(const Y&)>;

  /** Returns the f function */
  constexpr const f_type& f() const;

  /** Returns the g function */
  constexpr const g_type& g() const;

  /** Encodes the input */
  constexpr X g(const Y& y) const;

  /** Decodes the input */
  constexpr Y f(const X& x) const;

  /** Creates a codec from an f and g */
  constexpr function(f_type f, g_type g) noexcept;

 private:
  const f_type f_;
  const g_type g_;
};

template <typename X, typename Y>
constexpr const typename function<X, Y>::f_type& function<X, Y>::f() const {
  return f_;
}

template <typename X, typename Y>
constexpr const typename function<X, Y>::g_type& function<X, Y>::g() const {
  return g_;
}

template <typename X, typename Y>
constexpr X function<X, Y>::g(const Y& y) const {
  return g_(y);
}

template <typename X, typename Y>
constexpr Y function<X, Y>::f(const X& x) const {
  return f_(x);
}

template <typename X, typename Y>
constexpr function<X, Y>::function(function::f_type f,
                                   function::g_type g) noexcept
    : f_(std::move(f)), g_(std::move(g)) {}

}  // namespace blox::bijective
