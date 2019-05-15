#pragma once
#include <blox/datastore.h>
#include <map>

namespace blox {
class map : public datastore {
 public:
  std::pair<iterator, bool> insert(value_type value) override;
  size_type erase(key_type key) override;
  iterator find(key_type key) const override;
  iterator begin() const override;
  iterator end() const override;
  iterator insert(const_iterator iterator, const value_type& value) override;

 private:
  class cursor : public blox::cursor {
   public:
    using iterator = std::map<std::string, std::string>::const_iterator;
    explicit cursor(iterator it);
    std::string_view key() const override;
    std::string_view value() const override;
    bool equal(const blox::cursor& rhs) const override;
    void increment() override;
    void decrement() override;
    std::unique_ptr<blox::cursor> clone() const override;

    friend class map;

   private:
    iterator it_;
  };

  std::map<std::string, std::string, std::less<>> data_;
};
}  // namespace blox