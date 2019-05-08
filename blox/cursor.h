#pragma once
#include <string_view>

namespace blox {

/** Interface to iterate through values of a database */
class cursor {
 public:
  /** Get the current key */
  virtual std::string_view key() const = 0;

  /** Get the current value */
  virtual std::string_view value() const = 0;

  /** Compare for equality*/
  virtual bool equal(const cursor& rhs) const = 0;

  /** Move the cursor forwards */
  virtual void increment() = 0;

  /** Move the cursor backwards */
  virtual void decrement() = 0;
};
} // namespace blox