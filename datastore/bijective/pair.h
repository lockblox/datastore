#pragma once

#include <datastore/bijective/function.h>

namespace datastore::bijective {
/** Applies a pair of bijective functions to a tuple
 *
 * @tparam X A pair of values in domain of X
 * @tparam Y  A pair of values in the domain of Y
 */

template <typename X, typename Y>
class pair : public function<X, Y> {
 public:
  pair(function<typename X::first_type, typename Y::first_type> first,
       function<typename X::second_type, typename Y::second_type> second);
};

template <typename X, typename Y>
pair<X, Y>::pair(
    function<typename X::first_type, typename Y::first_type> first,
    function<typename X::second_type, typename Y::second_type> second)
    : function<X, Y>(
          [first, second](auto& x) {
            return Y{first.f(x.first), second.f(x.second)};
          },
          [first, second](auto& y) {
            return X{first.g(y.first), second.g(y.second)};
          }) {}

}  // namespace datastore::bijective
