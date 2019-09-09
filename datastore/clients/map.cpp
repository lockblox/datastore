#include "map.h"
#include <datastore/clients/detail/map.h>

namespace datastore::clients {

std::unique_ptr<client> make_map() {
  return std::make_unique<clients::detail::map>();
}

}  // namespace datastore::clients
