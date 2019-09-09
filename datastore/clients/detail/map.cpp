#include "map.h"

namespace datastore::clients::detail {

std::unique_ptr<client::cursor> map::erase(
    std::unique_ptr<client::cursor> pos) {
  auto cursor = dynamic_cast<const map::cursor*>(pos.get());
  if (cursor) {
    auto it = data_.erase(cursor->it_);
    pos = std::make_unique<map::cursor>(it);
  }
  return pos;
}

std::unique_ptr<client::cursor> map::lookup(key_type key) const {
  return std::make_unique<cursor>(data_.find(key));
}

std::unique_ptr<client::cursor> map::first() const {
  return std::make_unique<cursor>(data_.begin());
}

std::unique_ptr<client::cursor> map::last() const {
  return std::make_unique<cursor>(data_.end());
}

std::unique_ptr<client::cursor> map::insert_or_assign(
    std::unique_ptr<client::cursor> pos, const value_type& value) {
  auto cursor = dynamic_cast<const map::cursor*>(pos.get());
  if (cursor) {
    pos = std::make_unique<map::cursor>(data_.insert_or_assign(
        cursor->it_, std::string(value.first), std::string(value.second)));
  }
  return pos;
}

client::size_type map::capacity() const { return data_.max_size(); }

map::cursor::cursor(map::cursor::iterator it) : it_(it) {}

std::string_view map::cursor::key() const { return it_->first; }

std::string_view map::cursor::value() const { return it_->second; }

bool map::cursor::equal(const client::cursor& rhs) const {
  auto c = dynamic_cast<const map::cursor&>(rhs);
  return it_ == c.it_;
}

void map::cursor::increment() { ++it_; }

void map::cursor::decrement() { --it_; }

std::unique_ptr<client::cursor> map::cursor::clone() const {
  return std::make_unique<map::cursor>(it_);
}

}  // namespace datastore::clients::detail