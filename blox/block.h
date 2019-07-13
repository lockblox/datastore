#pragma once

#include <multihash/digest.h>
#include <multihash/function.h>
#include <string_view>

namespace blox {

/** A non-owning block of data identified by a unique cryptographic hash */
class block {
 public:
  using key_type = multihash::digest<std::string_view>;
  using mapped_type = std::string_view;

  /** Creates a block from an rvalue reference */
  block(block&& other) noexcept;

  /** Creates a block from a copy */
  block(const block& other);

  /** Assigns the block to a reference */
  block& operator=(const block& rhs);

  /** Assigns the block to an rvalue reference */
  block& operator=(block&& rhs) noexcept;

  /** Creates a block by hashing a data blob */
  explicit block(mapped_type data,
                 multihash::code_type hash_code = multihash::code::sha2_256);

  /** Creates a block from values */
  explicit block(key_type key, mapped_type data = mapped_type{});

  /** Returns the unique identifier of the block */
  key_type key() const;

  /** Returns the block data */
  mapped_type data() const;

 private:
  std::unique_ptr<char[]> buffer_;
  key_type key_;
  mapped_type data_;
};

/** Compares two blocks for inequality */
bool operator!=(const block& lhs, const block& rhs);

/** Compares two blocks for equality */
bool operator==(const block& lhs, const block& rhs);

/** Compares two blocks for ordering */
bool operator<(const block& lhs, const block& rhs);

/** Compares two blocks for ordering */
bool operator>(const block& lhs, const block& rhs);
}  // namespace blox
