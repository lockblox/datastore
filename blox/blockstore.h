#pragma once

#include <blox/block.h>
#include <blox/datastore.h>
#include <boost/iterator/transform_iterator.hpp>
#include <functional>

namespace blox {
class blockstore {
 public:
  // TODO blockstore is a set where a bijective function is provided for T,
  //  by deafault a bijective::stream
  using transform = std::function<block(datastore::const_reference)>;
  using key_type = block;
  using value_type = block;
  using size_type = datastore::size_type;
  using difference_type = datastore::difference_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = boost::transform_iterator<transform, datastore::iterator,
                                             value_type, value_type>;
  using const_iterator = const iterator;

  explicit blockstore(std::unique_ptr<datastore> datastore);
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
  [[nodiscard]] bool empty() const;
  /** Returns the number of blocks */
  [[nodiscard]] size_type size() const;
  /** Returns the maximum possible number of blocks */
  [[nodiscard]] size_type max_size() const;

  // Modifiers

  /** Clears the contents */
  void clear();
  /** Inserts elements */
  std::pair<iterator, bool> insert(const value_type& value);
  /** Inserts blocks */
  iterator insert(const_iterator, const value_type& value);
  /** Erases blocks */
  size_type erase(const key_type& key);

  // Lookup
  [[nodiscard]] iterator find(const key_type& key) const;

 private:
  std::unique_ptr<datastore> datastore_;
  transform transform_;
};

}  // namespace blox
