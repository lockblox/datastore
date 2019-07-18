#include <blox/datastores/map.h>

namespace blox::datastores {

std::unique_ptr<datastore::cursor> map::erase(
    std::unique_ptr<datastore::cursor>& pos) {
  auto cursor = dynamic_cast<const map::cursor*>(pos.get());
  if (cursor) {
    auto it = data_.erase(cursor->it_);
    pos = std::make_unique<map::cursor>(it);
  }
  return std::move(pos);
}

std::unique_ptr<datastore::cursor> map::lookup(datastore::key_type key) const {
  return std::make_unique<cursor>(data_.find(key));
}

std::unique_ptr<datastore::cursor> map::first() const {
  return std::make_unique<cursor>(data_.begin());
}

std::unique_ptr<datastore::cursor> map::last() const {
  return std::make_unique<cursor>(data_.end());
}

std::unique_ptr<datastore::cursor> map::insert(
    std::unique_ptr<datastore::cursor>& pos,
    const datastore::value_type& value) {
  auto cursor = dynamic_cast<const map::cursor*>(pos.get());
  if (cursor) {
    auto pair = std::pair(std::string(value.first), std::string(value.second));
    pos = std::make_unique<map::cursor>(data_.insert(cursor->it_, pair));
  }
  return std::move(pos);
}

map::cursor::cursor(map::cursor::iterator it) : it_(it) {}

std::string_view map::cursor::key() const { return it_->first; }

std::string_view map::cursor::value() const { return it_->second; }

bool map::cursor::equal(const datastore::cursor& rhs) const {
  auto c = dynamic_cast<const map::cursor&>(rhs);
  return it_ == c.it_;
}

void map::cursor::increment() { ++it_; }

void map::cursor::decrement() { --it_; }

std::unique_ptr<datastore::cursor> map::cursor::clone() const {
  return std::make_unique<map::cursor>(it_);
}
}  // namespace blox::datastores