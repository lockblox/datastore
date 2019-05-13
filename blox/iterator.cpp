#include <blox/iterator.h>

namespace blox {

iterator::value_type iterator::operator*() const {
  return std::pair(cursor_->key(), cursor_->value());
}

iterator::iterator(std::unique_ptr<cursor> cursor)
    : cursor_(std::move(cursor)) {}

iterator& iterator::operator++() {
  cursor_->increment();
  return *this;
}

bool iterator::operator==(const iterator& rhs) const {
  return cursor_ == rhs.cursor_ ||
         (cursor_ != nullptr && rhs.cursor_ != nullptr &&
          cursor_->equal(*rhs.cursor_));
}

bool iterator::operator!=(const iterator& rhs) const { return !(*this == rhs); }

iterator& iterator::operator--() {
  cursor_->decrement();
  return *this;
}
}  // namespace blox