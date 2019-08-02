#pragma once
#include <blox/datastore.h>
#include <filesystem>

namespace blox::datastores {

class lmdb_configuration {
 public:
  explicit lmdb_configuration(std::filesystem::path path,
                              unsigned int flags = 0, unsigned int mode = 0644);
  [[nodiscard]] const std::filesystem::path& path() const;
  [[nodiscard]] unsigned int flags() const;
  [[nodiscard]] unsigned int mode() const;

 private:
  std::filesystem::path path_;
  unsigned int flags_;
  unsigned int mode_;
};

/** Creates an lmdb datastore */
std::unique_ptr<datastore> make_lmdb(const lmdb_configuration& configuration);

}  // namespace blox::datastores
