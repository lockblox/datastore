#pragma once
#include <datastore/client.h>
#include <map>
#include "lmdb.h"

namespace datastore::clients::detail {

class map final : public client {
 public:
  using iterator = iterator;
  std::unique_ptr<client::cursor> erase(
      std::unique_ptr<client::cursor> pos) override;
  [[nodiscard]] std::unique_ptr<client::cursor> lookup(
      key_type key) const override;
  [[nodiscard]] std::unique_ptr<client::cursor> first() const override;
  [[nodiscard]] std::unique_ptr<client::cursor> last() const override;

 protected:
  [[nodiscard]] size_type capacity() const override;

 protected:
  std::unique_ptr<client::cursor> insert_or_assign(
      std::unique_ptr<client::cursor> cursor, const value_type& value) override;

 private:
  class cursor : public client::cursor {
   public:
    using iterator = std::map<std::string, std::string>::const_iterator;
    explicit cursor(iterator it);
    [[nodiscard]] std::string_view key() const override;
    [[nodiscard]] std::string_view value() const override;
    [[nodiscard]] bool equal(const client::cursor& rhs) const override;
    void increment() override;
    void decrement() override;
    [[nodiscard]] std::unique_ptr<client::cursor> clone() const override;

    friend class map;

   private:
    iterator it_;
  };

  std::map<std::string, std::string, std::less<>> data_;
};
}  // namespace datastore::clients::detail