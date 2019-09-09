#include <datastore/clients/map.h>
#include <datastore/map.h>
#include <gtest/gtest.h>

namespace test {

TEST(map, insert) {
  auto datastore = datastore::clients::make_map();
  datastore::map<int, double> map{*datastore};
  auto [it, inserted] = map.insert(std::pair{1, 2.0});
  EXPECT_TRUE(inserted);
  ASSERT_NE(map.end(), it);
  EXPECT_DOUBLE_EQ(2.0, it->second);
}

TEST(map, find) {
  auto datastore = datastore::clients::make_map();
  datastore::map<int, double> map{*datastore};
  map.insert(std::pair{1, 2.0});
  auto it = map.find(1);
  ASSERT_NE(map.end(), it);
  EXPECT_DOUBLE_EQ(2.0, it->second);
}

}  // namespace test
