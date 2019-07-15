#pragma once

#include <blox/block.h>
#include <blox/datastore.h>

namespace blox {
class blockstore {
 public:
  class iterator;
  using const_iterator = const iterator;

  using key_type = block;
  using value_type = block;
  using size_type = datastore::size_type;
  using difference_type = datastore::difference_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  explicit blockstore(std::unique_ptr<datastore> ds);
  blockstore(blockstore&&) = default;
  blockstore& operator=(blockstore&&) = default;
  ~blockstore() = default;

  /** Returns an iterator to the beginning */
  iterator begin();
  /** Returns an iterator to the beginning */
  const_iterator cbegin();
  /** Returns an iterator to the end */
  iterator end();
  /** Returns an iterator to the end */
  const_iterator cend();

  /** Checks whether the container is empty */
  bool empty();
  /** Returns the number of blocks */
  size_type size();
  /** Returns the maximum possible number of blocks */
  size_type max_size();

  // Modifiers

  /** Clears the contents */
  void clear();
  /** Inserts elements */
  std::pair<iterator, bool> insert(value_type value);
  /** Inserts blocks */
  iterator insert(const_iterator, const value_type& value);
  /** Erases blocks */
  size_type erase(const key_type& key);

  // Lookup
  iterator find(const key_type& key);

 private:
  std::unique_ptr<datastore> datastore_;
};

class blockstore::iterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = datastore::value_type;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;
  using reference = value_type&;

  /** Create a sentinel iterator */
  iterator() = default;

  /** Construct from a cursor */
  explicit iterator(std::unique_ptr<datastore::cursor> cursor);

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

 protected:
  std::unique_ptr<datastore::iterator> iterator_ =
      nullptr; /** Pointer to underlying iterator */
 private:
  const value_type& value() const;
};

}  // namespace blox
