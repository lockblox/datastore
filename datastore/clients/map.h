#pragma once
#include <datastore/client.h>
#include <memory>

namespace datastore::clients {

std::unique_ptr<client> make_map();

}  // namespace datastore::clients
