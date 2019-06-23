#include <lockblox/blox/datastore.h>

namespace lockblox::blox {

datastore::iterator::iterator(std::unique_ptr<datastore::cursor> cursor)
    : cursor_(std::move(cursor)) {}

const datastore::iterator::value_type* datastore::iterator::operator->() const {
  return &(value());
}

const datastore::iterator::value_type& datastore::iterator::operator*() const {
  return value();
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

datastore::iterator& datastore::iterator::operator++() {
  if (cursor_ != nullptr) {
    cursor_->increment();
    value_.reset();
  }
  return *this;
}

datastore::iterator& datastore::iterator::operator--() {
  if (cursor_ != nullptr) {
    cursor_->decrement();
    value_.reset();
  }
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

const datastore::iterator::value_type& datastore::iterator::value() const {
  if (!value_) {
    if (cursor_ != nullptr) {
      value_ = std::pair(cursor_->key(), cursor_->value());
    } else {
      value_ = datastore::iterator::value_type{};
    }
  }
  return *value_;
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
  for (const auto& i : *this) {
    erase(i.first);
  }
}

}  // namespace lockblox::blox
