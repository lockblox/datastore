#include <blox/datastores/map.h>

namespace blox::datastores {

datastore::iterator map::erase(datastore::iterator pos) {
  auto cursor = dynamic_cast<const map::cursor*>(pos.get());
  auto result = datastore::iterator();
  if (cursor) {
    auto it = data_.erase(cursor->it_);
    result = datastore::iterator(std::make_unique<map::cursor>(it));
  }
  return result;
}

datastore::iterator map::find(datastore::key_type key) const {
  return datastore::iterator(std::make_unique<cursor>(data_.find(key)));
}

std::unique_ptr<datastore::cursor> map::first() {
  return std::make_unique<cursor>(data_.begin());
}

std::unique_ptr<datastore::cursor> map::last() {
  return std::make_unique<cursor>(data_.end());
}

datastore::iterator map::insert(datastore::const_iterator iterator,
                                const datastore::value_type& value) {
  auto cursor = dynamic_cast<const map::cursor*>(iterator.get());
  auto result = map::iterator();
  if (cursor) {
    auto pair = std::pair(std::string(value.first), std::string(value.second));
    auto insertion = data_.insert(cursor->it_, pair);
    result =
        blox::datastore::iterator(std::make_unique<map::cursor>(insertion));
  }
  return result;
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