#pragma once
#include <boost/iterator/iterator_facade.hpp>
#include <memory>
#include <numeric>
#include <optional>
#include <string_view>
#include <utility>

namespace datastore {

/** Client driver for a key value database */
class client {
 public:
  class cursor;
  class iterator;
  using const_iterator = const iterator;
  using difference_type = std::ptrdiff_t;
  using key_type = std::string_view;
  using mapped_type = std::string_view;
  using value_type = std::pair<std::string_view, std::string_view>;
  using reference = std::add_lvalue_reference<value_type>::type;
  using const_reference =
      std::add_lvalue_reference<std::add_const<value_type>::type>::type;
  using size_type = std::size_t;

  virtual ~client() = default;

  // Element access

  /** Access specified element with bounds checking
   *
   * Returns a reference to the mapped value of the element with key equivalent
   * to key. If no such element exists, an exception of type std::out_of_range
   * is thrown. */
  const_reference at(const key_type& key);

  // Iterators

  /** Returns an iterator to the beginning */
  [[nodiscard]] const_iterator begin() const;
  /** Returns an iterator to the beginning */
  [[nodiscard]] const_iterator cbegin() const;

  /** Returns an iterator to the end */
  [[nodiscard]] const_iterator end() const;
  /** Returns an iterator to the end */
  [[nodiscard]] const_iterator cend() const;

  // Capacity

  /** Checks whether the db is empty */
  [[nodiscard]] virtual bool empty() const;

  /** Returns the number of elements in the db */
  [[nodiscard]] virtual size_type size() const;

  /** Returns the maximum possible number of elements in the db */
  [[nodiscard]] size_type max_size() const;

  // Modifiers

  /** Removes all elements from the db */
  virtual void clear();

  /** Inserts an element at the given position */
  iterator insert(iterator pos, const value_type& value);

  /** Inserts a value */
  std::pair<iterator, bool> insert(const value_type& value);

  /** Erases the value matching the given key */
  size_type erase(key_type key);

  /** Erases the element at pos */
  iterator erase(iterator pos);

  // Lookup

  /** Finds an element matching the given key */
  [[nodiscard]] iterator find(key_type key) const;

 protected:
  virtual std::unique_ptr<cursor> insert_or_assign(std::unique_ptr<cursor> pos,
                                                   const value_type& value) = 0;
  virtual std::unique_ptr<cursor> erase(std::unique_ptr<cursor> pos) = 0;
  [[nodiscard]] virtual std::unique_ptr<cursor> lookup(key_type key) const = 0;
  [[nodiscard]] virtual std::unique_ptr<cursor> first() const = 0;
  [[nodiscard]] virtual std::unique_ptr<cursor> last() const = 0;
  [[nodiscard]] virtual size_type capacity() const = 0;
};

/** Interface to iterate through values of a database */
class client::cursor {
 public:
  /** Destroy a cursor */
  virtual ~cursor() = default;

  [[nodiscard]] virtual std::unique_ptr<cursor> clone() const = 0;

  /** Get the current key */
  [[nodiscard]] virtual std::string_view key() const = 0;

  /** Get the current value */
  [[nodiscard]] virtual std::string_view value() const = 0;

  /** Compare for equality*/
  [[nodiscard]] virtual bool equal(const cursor& rhs) const = 0;

  /** Move the cursor forwards */
  virtual void increment() = 0;

  /** Move the cursor backwards */
  virtual void decrement() = 0;
};

class client::iterator
    : public boost::iterator_facade<client::iterator, const client::value_type,
                                    boost::bidirectional_traversal_tag> {
 public:
  using const_reference = std::add_const<reference>::type;

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

  /** Destroys the iterator */
  virtual ~iterator() = default;

  /** Moves the iterator forward */
  void increment();

  /** Moves the iterator backward */
  void decrement();

  /** Returns a reference to the value */
  const_reference dereference() const;

  /** Determine whether two iterators are equal */
  bool operator==(const iterator& rhs) const;

  /** Determine whether two iterators are unequal */
  bool operator!=(const iterator& rhs) const;

  friend class client;

 protected:
  std::unique_ptr<cursor> cursor_ = nullptr; /** Pointer to underlying cursor */
 private:
  mutable std::optional<value_type> value_;
};

}  // namespace datastore