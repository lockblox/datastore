#pragma once

#include <blox/bijective/stream.h>
#include <blox/datastore.h>
#include <boost/iterator/transform_iterator.hpp>
namespace blox {

template <typename Key>
class set {
 public:
  using key_transform_type = bijective::function<Key, std::string_view>;
  explicit set(blox::datastore& datastore,
               key_transform_type key_transform = bijective::stream<Key>{});

  using key_type = Key;
  using value_type = Key;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator =
      boost::transform_iterator<typename key_transform_type::g_type,
                                datastore::iterator, value_type, value_type>;

 private:
  blox::datastore* datastore_;
  key_transform_type key_transform_;
};

template <typename Key>
set<Key>::set(blox::datastore& datastore, set::key_transform_type key_transform)
    : datastore_(&datastore), key_transform_(std::move(key_transform)) {}

}  // namespace blox
