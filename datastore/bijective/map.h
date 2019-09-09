#pragma once

#include <datastore/bijective/function.h>
#include <datastore/bijective/pair.h>
#include <boost/iterator/transform_iterator.hpp>

namespace datastore::bijective {
/** Applies bijective functions to the key and mapped type of an
 * AssociativeContainer
 *
 * @tparam Key
 * @tparam T
 * @tparam AssociativeContainer
 *
 */
template <typename Key, typename T, typename AssociativeContainer>
class map {
 public:
  using value_type = std::pair<const Key, T>;

  using key_transform_type =
      function<Key, typename AssociativeContainer::key_type>;
  using mapped_transform_type =
      function<T, typename AssociativeContainer::mapped_type>;
  using value_transform_type =
      function<std::pair<Key, T>,
               std::pair<typename AssociativeContainer::key_type,
                         typename AssociativeContainer::mapped_type>>;

  using key_type = Key;
  using mapped_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator =
      boost::transform_iterator<typename value_transform_type::g_type,
                                typename AssociativeContainer::iterator,
                                value_type, value_type>;
  using const_iterator = typename std::add_const<iterator>::type;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  map(AssociativeContainer& container, key_transform_type key_transform,
      mapped_transform_type mapped_transform,
      value_transform_type value_transform) noexcept;

  map(AssociativeContainer& container, key_transform_type key_transform,
      mapped_transform_type mapped_transform) noexcept;
  // Iterators

  /** Returns an iterator to the beginning */
  [[nodiscard]] iterator begin() const;

  /** Returns an iterator to the beginning */
  [[nodiscard]] const_iterator cbegin() const;

  /** Returns an iterator to the end */
  [[nodiscard]] iterator end() const;
  /** Returns an iterator to the end */
  [[nodiscard]] const_iterator cend() const;

  // Capacity

  /** Checks whether the datastore is empty */
  [[nodiscard]] virtual bool empty() const;

  /** Returns the number of elements in the datastore */
  [[nodiscard]] virtual size_type size() const;

  /** Returns the maximum possible number of elements in the datastore */
  [[nodiscard]] size_type max_size() const;

  // Modifiers

  /** Removes all elements from the datastore */
  virtual void clear();

  /** Inserts an element at the given position */
  iterator insert(iterator pos, const value_type& value);

  /** Inserts a value */
  std::pair<iterator, bool> insert(const value_type& value);

  /** Erases the value matching the given key */
  size_type erase(const key_type& key);

  /** Erases the element at pos */
  iterator erase(iterator pos);

  // Lookup

  /** Finds an element matching the given key */
  [[nodiscard]] iterator find(const key_type& key) const;

 private:
  AssociativeContainer* container_;
  key_transform_type key_transform_;
  mapped_transform_type mapped_transform_;
  value_transform_type value_transform_;
};

template <typename Key, typename T, typename AssociativeContainer>
map<Key, T, AssociativeContainer>::map(
    AssociativeContainer& container,
    map<Key, T, AssociativeContainer>::key_transform_type key_transform,
    map<Key, T, AssociativeContainer>::mapped_transform_type mapped_transform,
    map<Key, T, AssociativeContainer>::value_transform_type
        value_transform) noexcept
    : container_(&container),
      key_transform_(std::move(key_transform)),
      mapped_transform_(std::move(mapped_transform)),
      value_transform_(std::move(value_transform)) {}

template <typename Key, typename T, typename AssociativeContainer>
map<Key, T, AssociativeContainer>::map(
    AssociativeContainer& container,
    map<Key, T, AssociativeContainer>::key_transform_type key_transform,
    map<Key, T, AssociativeContainer>::mapped_transform_type
        mapped_transform) noexcept
    : container_(&container),
      key_transform_(key_transform),
      mapped_transform_(mapped_transform),
      value_transform_(
          pair<std::pair<Key, T>,
               std::pair<typename AssociativeContainer::key_type,
                         typename AssociativeContainer::mapped_type>>{
              key_transform, mapped_transform}) {}

template <typename Key, typename T, typename AssociativeContainer>
typename map<Key, T, AssociativeContainer>::iterator
map<Key, T, AssociativeContainer>::begin() const {
  return iterator(container_->begin(), value_transform_.g());
}

template <typename Key, typename T, typename AssociativeContainer>
typename map<Key, T, AssociativeContainer>::const_iterator
map<Key, T, AssociativeContainer>::cbegin() const {
  return iterator(container_->cbegin(), value_transform_.g());
}

template <typename Key, typename T, typename AssociativeContainer>
typename map<Key, T, AssociativeContainer>::iterator
map<Key, T, AssociativeContainer>::end() const {
  return iterator(container_->end(), value_transform_.g());
}

template <typename Key, typename T, typename AssociativeContainer>
typename map<Key, T, AssociativeContainer>::const_iterator
map<Key, T, AssociativeContainer>::cend() const {
  return iterator(container_->cend(), value_transform_.g());
}

template <typename Key, typename T, typename AssociativeContainer>
bool map<Key, T, AssociativeContainer>::empty() const {
  return container_->empty();
}

template <typename Key, typename T, typename AssociativeContainer>
typename map<Key, T, AssociativeContainer>::size_type
map<Key, T, AssociativeContainer>::size() const {
  return container_->size();
}

template <typename Key, typename T, typename AssociativeContainer>
typename map<Key, T, AssociativeContainer>::size_type
map<Key, T, AssociativeContainer>::max_size() const {
  return container_->max_size();
}

template <typename Key, typename T, typename AssociativeContainer>
void map<Key, T, AssociativeContainer>::clear() {
  container_->clear();
}

template <typename Key, typename T, typename AssociativeContainer>
typename map<Key, T, AssociativeContainer>::iterator
map<Key, T, AssociativeContainer>::insert(map::iterator pos,
                                          const map::value_type& value) {
  return iterator(container_->insert(pos.base(), value_transform_.f(value)),
                  value_transform_.g());
}

template <typename Key, typename T, typename AssociativeContainer>
std::pair<typename map<Key, T, AssociativeContainer>::iterator, bool>
map<Key, T, AssociativeContainer>::insert(const value_type& value) {
  auto transformed_value = value_transform_.f(value);
  auto insert_result = container_->insert(transformed_value);
  return std::pair(iterator(insert_result.first, value_transform_.g()),
                   insert_result.second);
}

template <typename Key, typename T, typename AssociativeContainer>
typename map<Key, T, AssociativeContainer>::size_type
map<Key, T, AssociativeContainer>::erase(const key_type& key) {
  return container_->erase(key_transform_.f(key));
}

template <typename Key, typename T, typename AssociativeContainer>
typename map<Key, T, AssociativeContainer>::iterator
map<Key, T, AssociativeContainer>::erase(iterator pos) {
  return iterator(container_->erase(pos.base()), value_transform_.g());
}

template <typename Key, typename T, typename AssociativeContainer>
typename map<Key, T, AssociativeContainer>::iterator
map<Key, T, AssociativeContainer>::find(const key_type& key) const {
  return iterator(container_->find(key_transform_.f(key)),
                  value_transform_.g());
}

}  // namespace datastore::bijective
