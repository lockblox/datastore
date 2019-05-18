#include <lockblox/blox/datastore.h>

namespace lockblox::blox {

datastore::iterator::iterator(std::unique_ptr<datastore::cursor> cursor)
    : cursor_(std::move(cursor)) {
}

const datastore::iterator::value_type* datastore::iterator::operator->() const {
  return &(value());
}

const datastore::iterator::value_type& datastore::iterator::operator*() const {
  return value();
}

datastore::iterator::iterator(const iterator& rhs)
    : iterator(rhs.cursor_->clone()) {}

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
  cursor_->increment();
  value_.reset();
  return *this;
}

datastore::iterator& datastore::iterator::operator--() {
  cursor_->decrement();
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

const datastore::iterator::value_type& datastore::iterator::value() const {
  if (!value_) {
    value_ = std::pair(cursor_->key(), cursor_->value());
  }
  return *value_;
}

}  // namespace lockblox::blox
