#pragma once
#include <blox/datastore.h>
#include <map>
#include "lmdb.h"

namespace blox::datastores::impl {

class map final : public datastore {
 public:
  using iterator = iterator;
  std::unique_ptr<datastore::cursor> erase(
      std::unique_ptr<datastore::cursor> pos) override;
  [[nodiscard]] std::unique_ptr<datastore::cursor> lookup(
      key_type key) const override;
  [[nodiscard]] std::unique_ptr<datastore::cursor> first() const override;
  [[nodiscard]] std::unique_ptr<datastore::cursor> last() const override;

 protected:
  [[nodiscard]] size_type capacity() const override;

 protected:
  std::unique_ptr<datastore::cursor> insert(
      std::unique_ptr<datastore::cursor> cursor,
      const value_type& value) override;

 private:
  class cursor : public datastore::cursor {
   public:
    using iterator = std::map<std::string, std::string>::const_iterator;
    explicit cursor(iterator it);
    [[nodiscard]] std::string_view key() const override;
    [[nodiscard]] std::string_view value() const override;
    [[nodiscard]] bool equal(const datastore::cursor& rhs) const override;
    void increment() override;
    void decrement() override;
    [[nodiscard]] std::unique_ptr<datastore::cursor> clone() const override;

    friend class map;

   private:
    iterator it_;
  };

  std::map<std::string, std::string, std::less<>> data_;
};
}  // namespace blox::datastores::impl