#pragma once
#include <blox/datastore.h>
#include <map>
#include "lmdb.h"

namespace blox::datastores {

class map final : public datastore {
 public:
  using iterator = datastore::iterator;
  iterator erase(iterator pos) override;
  iterator find(key_type key) const override;
  iterator begin() const override;
  iterator end() const override;
  iterator insert(const_iterator iterator, const value_type& value) override;

 private:
  class cursor : public datastore::cursor {
   public:
    using iterator = std::map<std::string, std::string>::const_iterator;
    explicit cursor(iterator it);
    std::string_view key() const override;
    std::string_view value() const override;
    bool equal(const datastore::cursor& rhs) const override;
    void increment() override;
    void decrement() override;
    std::unique_ptr<datastore::cursor> clone() const override;

    friend class map;

   private:
    iterator it_;
  };

  std::map<std::string, std::string, std::less<>> data_;
};
}  // namespace blox::datastores