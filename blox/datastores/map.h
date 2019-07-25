#pragma once
#include <blox/datastore.h>
#include <map>
#include "lmdb.h"

namespace blox::datastores {

class map final : public datastore {
 public:
  using iterator = datastore::iterator;
  std::unique_ptr<datastore::cursor> erase(
      std::unique_ptr<datastore::cursor>& pos) override;
  std::unique_ptr<datastore::cursor> lookup(key_type key) const override;
  std::unique_ptr<datastore::cursor> first() const override;
  std::unique_ptr<datastore::cursor> last() const override;

 protected:
  size_type capacity() const override;

 protected:
  std::unique_ptr<cursor> insert(std::unique_ptr<cursor>& cursor,
                                 const value_type& value) override;

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