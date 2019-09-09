#pragma once

#include <datastore/bijective/function.h>
#include <datastore/bijective/map.h>
#include <datastore/bijective/stream.h>
#include <datastore/client.h>
#include <functional>

namespace datastore {

/** map provides an std::map like interface atop a datastore */
template <typename Key, typename T>
class map : public datastore::bijective::map<Key, T, client> {
 public:
  using base_type = bijective::map<Key, T, client>;
  using typename base_type::key_transform_type;
  using typename base_type::mapped_transform_type;
  explicit map(client& datastore,
               key_transform_type key_transform = bijective::stream<Key>{},
               mapped_transform_type mapped_transform = bijective::stream<T>{});
};

template <typename Key, typename T>
map<Key, T>::map(client& datastore,
                 typename map<Key, T>::key_transform_type key_transform,
                 typename map<Key, T>::mapped_transform_type mapped_transform)
    : base_type(datastore, key_transform, mapped_transform) {}

}  // namespace datastore
