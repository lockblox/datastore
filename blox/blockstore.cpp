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

blockstore::size_type blockstore::max_size() const {
  return datastore_->max_size();
}

blockstore::iterator blockstore::insert(blockstore::const_iterator pos,
                                        const blockstore::value_type& value) {
  auto key = static_cast<std::string_view>(value.key());
  auto kv = std::pair(key, value.data());
  auto result = datastore_->insert(pos.base(), kv);
  return iterator(result);
}

blockstore::iterator blockstore::find(const blockstore::key_type& key) const {
  return iterator(datastore_->find(static_cast<std::string_view>(key.key())));
}

blockstore::size_type blockstore::erase(const blockstore::key_type& key) {
  return datastore_->erase(static_cast<std::string_view>(key.key()));
}

}  // namespace blox
