#pragma once

#include <blox/bijective/stream.h>
#include <blox/datastore.h>
#include <functional>
#include "blox/bijective/function.h"
#include "blox/bijective/map.h"

namespace blox {

template <typename Key, typename T>
class map : public bijective::map<Key, T, datastore> {
 public:
  using base_type = bijective::map<Key, T, datastore>;
  using typename base_type::key_transform_type;
  using typename base_type::mapped_transform_type;
  explicit map(blox::datastore& datastore,
               key_transform_type key_transform = bijective::stream<Key>{},
               mapped_transform_type mapped_transform = bijective::stream<T>{});
};

template <typename Key, typename T>
map<Key, T>::map(blox::datastore& datastore,
                 typename map<Key, T>::key_transform_type key_transform,
                 typename map<Key, T>::mapped_transform_type mapped_transform)
    : base_type(datastore, key_transform, mapped_transform) {}

}  // namespace blox
