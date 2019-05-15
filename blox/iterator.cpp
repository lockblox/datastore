#include <blox/iterator.h>

namespace blox {

iterator::iterator(std::unique_ptr<cursor> cursor)
    : cursor_(std::move(cursor)) {
  if (cursor_ != nullptr) {
    value_ = std::pair(cursor_->key(), cursor_->value());
  }
}

const iterator::value_type* iterator::operator->() const { return &(value()); }

const iterator::value_type& iterator::operator*() const { return value(); }

iterator::iterator(const iterator& rhs) : iterator(rhs.cursor_->clone()) {}

iterator& iterator::operator=(const iterator& rhs) {
  if (rhs.cursor_ != nullptr) {
    cursor_ = rhs.cursor_->clone();
  } else {
    cursor_ = nullptr;
  }
  value_.reset();
  return *this;
}

iterator& iterator::operator++() {
  cursor_->increment();
  value_.reset();
  return *this;
}

iterator& iterator::operator--() {
  cursor_->decrement();
  value_.reset();
  return *this;
}

bool iterator::operator==(const iterator& rhs) const {
  return cursor_ == rhs.cursor_ ||
         (cursor_ != nullptr && rhs.cursor_ != nullptr &&
          cursor_->equal(*rhs.cursor_));
}

bool iterator::operator!=(const iterator& rhs) const { return !(*this == rhs); }

cursor* iterator::get() { return cursor_.get(); }

const cursor* iterator::get() const { return cursor_.get(); }

const iterator::value_type& iterator::value() const {
  if (!value_) {
    value_ = std::pair(cursor_->key(), cursor_->value());
  }
  return *value_;
}
}  // namespace blox