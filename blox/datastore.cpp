#include <blox/datastore.h>

namespace blox {

datastore::iterator::iterator(std::unique_ptr<datastore::cursor> cursor)
    : cursor_(std::move(cursor)) {}

datastore::const_iterator datastore::begin() const {
  return iterator(const_cast<datastore*>(this)->first());
}

datastore::const_iterator datastore::cbegin() const { return begin(); }

datastore::const_iterator datastore::end() const {
  return iterator(const_cast<datastore*>(this)->last());
}

datastore::const_iterator datastore::cend() const { return end(); }

datastore::iterator::const_reference datastore::iterator::dereference() const {
  if (!value_) {
    if (cursor_ != nullptr) {
      value_ = std::pair(cursor_->key(), cursor_->value());
    } else {
      value_ = datastore::iterator::value_type{};
    }
  }
  return *value_;
}

datastore::iterator::iterator(const iterator& rhs)
    : iterator(rhs.cursor_ ? rhs.cursor_->clone() : nullptr) {}

datastore::iterator& datastore::iterator::operator=(
    const datastore::iterator& rhs) {
  if (rhs.cursor_ != nullptr) {
    cursor_ = rhs.cursor_->clone();
  } else {
    cursor_ = nullptr;
  }
  value_.reset();
  return *this;
}

bool datastore::iterator::operator==(const iterator& rhs) const {
  return cursor_ == rhs.cursor_ ||
         (cursor_ != nullptr && rhs.cursor_ != nullptr &&
          cursor_->equal(*rhs.cursor_));
}

bool datastore::iterator::operator!=(const iterator& rhs) const {
  return !(*this == rhs);
}

datastore::cursor* datastore::iterator::get() { return cursor_.get(); }

const datastore::cursor* datastore::iterator::get() const {
  return cursor_.get();
}

void datastore::iterator::increment() {
  cursor_->increment();
  value_.reset();
}

void datastore::iterator::decrement() {
  cursor_->decrement();
  value_.reset();
}

datastore::size_type datastore::erase(datastore::key_type key) {
  auto result = 0;
  auto it = find(key);
  if (it != end()) {
    erase(it);
    result = 1;
  }
  return result;
}

std::pair<datastore::iterator, bool> datastore::insert(
    datastore::value_type value) {
  auto result = std::pair(find(value.first), false);
  if (result.first == end()) {
    result = std::pair(insert(result.first, value), true);
  }
  return result;
}

void datastore::clear() {
  for (auto&& i : *this) {
    erase(i.first);
  }
}

}  // namespace blox
