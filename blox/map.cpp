#include <blox/map.h>

namespace blox {

std::pair<iterator, bool> map::insert(datastore::value_type value) {
  auto pair = std::pair(std::string(value.first), std::string(value.second));
  auto result = data_.insert(pair);
  return std::pair(iterator(std::make_unique<cursor>(result.first)),
                   result.second);
}

datastore::size_type map::erase(datastore::key_type key) {
  auto it = data_.find(key);
  auto result = it == data_.end() ? 0 : 1u;
  data_.erase(it);
  return result;
}

iterator map::find(datastore::key_type key) const {
  return iterator(std::make_unique<cursor>(data_.find(key)));
}

iterator map::begin() const {
  return iterator(std::make_unique<cursor>(data_.begin()));
}

iterator map::end() const {
  return iterator(std::make_unique<cursor>(data_.end()));
}

iterator map::insert(datastore::const_iterator iterator,
                     const datastore::value_type& value) {
  auto cursor = dynamic_cast<const map::cursor*>(iterator.get());
  auto result = map::iterator();
  if (cursor) {
    auto pair = std::pair(std::string(value.first), std::string(value.second));
    auto insertion = data_.insert(cursor->it_, pair);
    result = blox::iterator(std::make_unique<map::cursor>(insertion));
  }
  return result;
}

map::cursor::cursor(map::cursor::iterator it) : it_(it) {}

std::string_view map::cursor::key() const { return it_->first; }

std::string_view map::cursor::value() const { return it_->second; }

bool map::cursor::equal(const blox::cursor& rhs) const {
  auto c = dynamic_cast<const map::cursor&>(rhs);
  return it_ == c.it_;
}

void map::cursor::increment() { ++it_; }

void map::cursor::decrement() { --it_; }

std::unique_ptr<cursor> map::cursor::clone() const {
  return std::make_unique<map::cursor>(it_);
}
}  // namespace blox