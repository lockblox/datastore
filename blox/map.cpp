#include <blox/map.h>

namespace blox {

std::pair<iterator, bool> map::insert(datastore::value_type value) {
  auto result = data_.insert(std::pair(std::string(value.first),
                                       std::string(value.second)));
  auto it = iterator(std::make_unique<cursor>(result.first));
  return std::pair(std::move(it), result.second);
}

datastore::size_type map::erase(datastore::key_type key) {
  return data_.erase(std::string(key));
}

iterator map::find(datastore::key_type key) const {
  return iterator(std::make_unique<cursor>(data_.find(std::string(key))));
}

iterator map::begin() const {
  return iterator(std::make_unique<cursor>(data_.begin()));
}

iterator map::end() const {
  return iterator(std::make_unique<cursor>(data_.end()));
}

map::cursor::cursor(map::cursor::iterator it)
    : it_(it) {
}

std::string_view map::cursor::key() const {
  return std::string_view();
}

std::string_view map::cursor::value() const {
  return std::string_view();
}

bool map::cursor::equal(const blox::cursor& rhs) const {
  auto c = dynamic_cast<const map::cursor&>(rhs);
  return it_ == c.it_;
}

void map::cursor::increment() {
  ++it_;
}

void map::cursor::decrement() {
  --it_;
}
}