#pragma once
#include <blox/iterator.h>
#include <string_view>
#include <utility>

namespace blox {

class datastore {
 public:
  virtual ~datastore() = default;

  using iterator = blox::iterator;
  using const_iterator = const iterator;
  using key_type = std::string_view;
  using value_type = std::pair<std::string_view, std::string_view>;
  using size_type = std::size_t;

  virtual iterator begin() const = 0;

  virtual iterator end() const = 0;

  /** Insert a value */
  virtual std::pair<iterator, bool> insert(value_type value) = 0;

  /** Erase the value matching the given key */
  virtual size_type erase(key_type key) = 0;

  /** Find a value matching the given key */
  virtual iterator find(key_type key) const = 0;
};
}  // namespace blox