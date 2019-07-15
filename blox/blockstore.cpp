#include <blox/blockstore.h>

namespace blox {

blox::blockstore::blockstore(std::unique_ptr<blox::datastore> ds)
    : datastore_(std::move(ds)) {}


}  // namespace blox
