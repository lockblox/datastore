#include <blox/datastores/impl/lmdb.h>

namespace blox::datastores {

lmdb_configuration::lmdb_configuration(std::filesystem::path path,
                                       unsigned int flags, unsigned int mode)
    : path_(std::move(path)), flags_(flags), mode_(mode) {}

const std::filesystem::path& lmdb_configuration::path() const { return path_; }

unsigned int lmdb_configuration::flags() const { return flags_; }

unsigned int lmdb_configuration::mode() const { return mode_; }

std::unique_ptr<datastore> make_lmdb(const lmdb_configuration& configuration) {
  return std::make_unique<impl::lmdb>(configuration);
}
}  // namespace blox::datastores
