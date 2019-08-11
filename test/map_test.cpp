#include <blox/datastores/map.h>
#include <blox/map.h>
#include <gtest/gtest.h>

namespace test {

TEST(map, insert) {
  auto datastore = blox::datastores::make_map();
  blox::map<int, double> map{*datastore};
  auto [it, inserted] = map.insert(std::pair{1, 2.0});
  EXPECT_TRUE(inserted);
  ASSERT_NE(map.end(), it);
  EXPECT_DOUBLE_EQ(2.0, it->second);
}

TEST(map, find) {
  auto datastore = blox::datastores::make_map();
  blox::map<int, double> map{*datastore};
  map.insert(std::pair{1, 2.0});
  auto it = map.find(1);
  ASSERT_NE(map.end(), it);
  EXPECT_DOUBLE_EQ(2.0, it->second);
}

}  // namespace test
