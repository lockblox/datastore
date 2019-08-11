#pragma once

#include <blox/datastore.h>
#include <blox/stream_codec.h>
#include <functional>
#include "blox/bijective/function.h"
#include "blox/bijective/map.h"

namespace blox {

template <typename Key, typename T>
class map : public bijective::map<Key, T, datastore> {
 public:
  using base_type = bijective::map<Key, T, datastore>;
  explicit map(blox::datastore& datastore,
               typename base_type::key_transform_type key_transform =
                   stream_codec<Key>{},
               typename base_type::mapped_transform_type mapped_transform =
                   stream_codec<T>{});
};

template <typename Key, typename T>
map<Key, T>::map(
    blox::datastore& datastore,
    typename map<Key, T>::base_type::key_transform_type key_transform,
    typename map<Key, T>::base_type::mapped_transform_type mapped_transform)

    : base_type(datastore, key_transform, mapped_transform) {}

}  // namespace blox
