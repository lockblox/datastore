#pragma once
#include <blox/datastore.h>
#include <memory>

namespace blox::datastores {

std::unique_ptr<datastore> make_map();

}  // namespace blox::datastores
