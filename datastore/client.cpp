#include <datastore/client.h>
#include <stdexcept>

namespace datastore {

client::iterator::iterator(std::unique_ptr<client::cursor> cursor)
    : cursor_(std::move(cursor)) {}

client::const_iterator client::begin() const {
  return iterator(const_cast<client*>(this)->first());
}

client::const_iterator client::cbegin() const { return begin(); }

client::const_iterator client::end() const {
  return iterator(const_cast<client*>(this)->last());
}

client::const_iterator client::cend() const { return end(); }

client::iterator::const_reference client::iterator::dereference() const {
  if (!value_) {
    if (cursor_ != nullptr) {
      value_ = std::pair(cursor_->key(), cursor_->value());
    } else {
      value_ = client::iterator::value_type{};
    }
  }
  return *value_;
}

client::iterator::iterator(const iterator& rhs)
    : iterator(rhs.cursor_ ? rhs.cursor_->clone() : nullptr) {}

client::iterator& client::iterator::operator=(const client::iterator& rhs) {
  if (rhs.cursor_ != nullptr) {
    cursor_ = rhs.cursor_->clone();
  } else {
    cursor_ = nullptr;
  }
  value_.reset();
  return *this;
}

bool client::iterator::operator==(const iterator& rhs) const {
  return cursor_ == rhs.cursor_ ||
         (cursor_ != nullptr && rhs.cursor_ != nullptr &&
          cursor_->equal(*rhs.cursor_));
}

bool client::iterator::operator!=(const iterator& rhs) const {
  return !(*this == rhs);
}

void client::iterator::increment() {
  cursor_->increment();
  value_.reset();
}

void client::iterator::decrement() {
  cursor_->decrement();
  value_.reset();
}

client::size_type client::erase(client::key_type key) {
  auto result = 0;
  auto it = find(key);
  if (it != end()) {
    erase(std::move(it));
    result = 1;
  }
  return result;
}

client::size_type client::max_size() const { return capacity(); }

client::iterator client::insert(client::iterator pos,
                                const client::value_type& value) {
  auto it = find(value.first);
  if (it == end()) {
    it = iterator(insert_or_assign(std::move(pos.cursor_), value));
  }
  return it;
}

client::iterator client::erase(client::iterator pos) {
  return iterator(erase(std::move(pos.cursor_)));
}

client::iterator client::find(client::key_type key) const {
  return iterator(lookup(key));
}

const client::value_type& client::at(const client::key_type& key) {
  auto it = find(key);
  if (it == end()) {
    throw std::out_of_range{"key not found"};
  } else {
    return *it;
  }
}

std::pair<client::iterator, bool> client::insert(
    const client::value_type& value) {
  auto result = std::pair(find(value.first), false);
  if (result.first == end()) {
    result = std::pair(insert(result.first, value), true);
  }
  return result;
}

void client::clear() {
  for (auto&& i : *this) {
    erase(i.first);
  }
}

client::size_type client::size() const {
  return std::accumulate(begin(), end(), size_type{0},
                         [](size_type x, auto&) { return x + 1; });
}

bool client::empty() const { return begin() == end(); }
}  // namespace datastore
