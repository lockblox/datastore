#pragma once
#include <blox/cursor.h>
#include <cstddef>
#include <iterator>
#include <memory>
#include <optional>
#include <string_view>

namespace blox {

class iterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = std::pair<std::string_view, std::string_view>;
  using difference_type = std::ptrdiff_t;
  using pointer = std::string_view*;
  using reference = std::string_view&;

  /** Create a sentinel iterator */
  iterator() = default;

  /** Construct from a cursor */
  explicit iterator(std::unique_ptr<cursor> cursor);

  /** Construct a copy */
  iterator(const iterator& rhs);

  /** Assign a copy */
  iterator& operator=(const iterator& rhs);

  /** Construct from rvalue reference */
  iterator(iterator&&) = default;

  /** Assign from rvalue reference */
  iterator& operator=(iterator&&) = default;

  virtual ~iterator() = default;

  /** Access the current value */
  const value_type* operator->() const;

  /** Get the current value */
  const value_type& operator*() const;

  /** Pre-increment */
  iterator& operator++();

  /** Pre-decrement */
  iterator& operator--();

  /** Determine whether two iterators are equal */
  bool operator==(const iterator& rhs) const;

  /** Determine whether two iterators are unequal */
  bool operator!=(const iterator& rhs) const;

  cursor* get();

  const cursor* get() const;

 protected:
  std::unique_ptr<cursor> cursor_ = nullptr; /** Pointer to underlying cursor */
 private:
  mutable std::optional<value_type> value_;

  const value_type& value() const;
};
}  // namespace blox