#include <blox/blockstore.h>

namespace blox {

blox::blockstore::blockstore(std::unique_ptr<blox::datastore> datastore)
    : datastore_(std::move(datastore)), transform_([](auto& x) {
        return block{block::key_type{x.first}, x.second};
      }) {}

blockstore::iterator blockstore::begin() {
  return blox::blockstore::iterator(datastore_->begin(), transform_);
}

blockstore::const_iterator blockstore::cbegin() {
  return blox::blockstore::const_iterator(datastore_->cbegin(), transform_);
}

blockstore::iterator blockstore::end() {
  return blox::blockstore::iterator(datastore_->end(), transform_);
}

blockstore::const_iterator blockstore::cend() {
  return blox::blockstore::const_iterator(datastore_->cend(), transform_);
}

bool blockstore::empty() const { return datastore_->empty(); }

blockstore::size_type blockstore::size() const { return datastore_->size(); }

void blockstore::clear() { datastore_->clear(); }

std::pair<blockstore::iterator, bool> blockstore::insert(
    const blockstore::value_type& value) {
  auto key = std::string_view(value.key().data(), value.key().size());
  auto kv = std::pair(key, value.data());
  auto result = datastore_->insert(kv);
  return std::pair(iterator(result.first), result.second);
}

}  // namespace blox
