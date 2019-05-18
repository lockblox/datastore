#pragma once
#include <memory>
#include <optional>
#include <string_view>
#include <utility>

namespace lockblox::blox {

class datastore {
 public:
  class cursor;
  class iterator;
  using const_iterator = const iterator;
  using key_type = std::string_view;
  using value_type = std::pair<std::string_view, std::string_view>;
  using size_type = std::size_t;

  virtual ~datastore() = default;

  virtual iterator begin() const = 0;

  virtual iterator end() const = 0;

  /** Insert a value */
  virtual std::pair<iterator, bool> insert(value_type value) = 0;

  /** Insert an element at the given position */
  virtual iterator insert(const_iterator, const value_type& value) = 0;

  /** Erase the value matching the given key */
  virtual size_type erase(key_type key) = 0;

  /** Find a value matching the given key */
  virtual iterator find(key_type key) const = 0;
};

/** Interface to iterate through values of a database */
class datastore::cursor {
 public:
  /** Destroy a cursor */
  virtual ~cursor() = default;

  virtual std::unique_ptr<cursor> clone() const = 0;

  /** Get the current key */
  virtual std::string_view key() const = 0;

  /** Get the current value */
  virtual std::string_view value() const = 0;

  /** Compare for equality*/
  virtual bool equal(const cursor& rhs) const = 0;

  /** Move the cursor forwards */
  virtual void increment() = 0;

  /** Move the cursor backwards */
  virtual void decrement() = 0;
};

class datastore::iterator {
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

}  // namespace lockblox::blox