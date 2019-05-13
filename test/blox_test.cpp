#include <blox/map.h>
#include <gtest/gtest.h>
#include <sstream>

TEST(blox, map) {
  auto map = blox::map();
  EXPECT_EQ(map.end(), map.find("a"));
  map.insert(std::pair("a", "1"));
  EXPECT_NE(map.end(), map.find("a"));
  map.erase("a");
  EXPECT_EQ(map.end(), map.find("a"));
}
