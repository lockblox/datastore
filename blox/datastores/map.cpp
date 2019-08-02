#include "map.h"
#include <blox/datastores/impl/map.h>

namespace blox::datastores {

std::unique_ptr<datastore> make_map() { return std::make_unique<impl::map>(); }

}  // namespace blox::datastores
