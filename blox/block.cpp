#include <memory>

#include <blox/block.h>

namespace blox {

block::block(block&& other) noexcept
    : buffer_(std::move(other.buffer_)), key_(other.key_), data_(other.data_) {}

block::block(const block& other) : buffer_(nullptr) { *this = other; }

block& block::operator=(const block& rhs) {
  if (&rhs != this) {
    if (rhs.buffer_) {
      buffer_ = std::make_unique<char[]>(rhs.key().size());
      std::copy(rhs.key().data(), rhs.key().data() + rhs.key().size(),
                buffer_.get());
      key_ = key_type{std::string_view{buffer_.get(), rhs.key().size()}};
    } else {
      key_ = rhs.key_;
    }
    data_ = rhs.data_;
  }
  return *this;
}

block& block::operator=(block&& rhs) noexcept {
  buffer_ = std::move(rhs.buffer_);
  key_ = rhs.key_;
  data_ = rhs.data_;
  return *this;
}

block::block(block::mapped_type data, multihash::code_type hash_code)
    : buffer_(nullptr), data_(data) {
  auto hash_function = multihash::function{hash_code};
  buffer_ = std::make_unique<char[]>(hash_function.size());
  key_ = std::string_view(buffer_.get(), hash_function.size());
  hash_function(data.cbegin(), data.cend(), buffer_.get());
}

block::block(block::key_type key, block::mapped_type data)
    : key_(key), data_(data) {}

block::block(std::pair<const std::string_view, std::string_view> value)
    : key_(value.first), data_(value.second) {}

block::block(std::pair<std::string_view, std::string_view> value)
    : key_(value.first), data_(value.second) {}

blox::block::key_type blox::block::key() const { return key_; }

block::mapped_type block::data() const { return data_; }

bool block::empty() const { return data_.empty(); }

block::operator std::pair<std::string_view, std::string_view>() const {
  return std::pair(static_cast<std::string_view>(key_), data_);
}

block::operator std::pair<const std::string_view, std::string_view>() const {
  return std::pair(static_cast<std::string_view>(key_), data_);
}

bool operator==(const block& lhs, const block& rhs) {
  return !(lhs < rhs) && !(rhs < lhs);
}

bool operator!=(const block& lhs, const block& rhs) {
  return lhs < rhs || rhs < lhs;
}

bool operator<(const block& lhs, const block& rhs) {
  if ((lhs.data().empty() && !lhs.key().empty()) ||
      (rhs.data().empty() && !rhs.key().empty())) {
    return lhs.key() < rhs.key();  // compare keys when data is not present
  } else if (lhs.key().empty() || rhs.key().empty()) {
    return lhs.data() < rhs.data();  // compare data when key is missing
  } else {
    return lhs.key() < rhs.key();  // default to key comparison
  }
}

bool operator>(const block& lhs, const block& rhs) { return rhs < lhs; }

}  // namespace blox