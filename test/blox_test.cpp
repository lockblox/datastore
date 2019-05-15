#include <blox/map.h>
#include <gtest/gtest.h>
#include <sstream>

namespace test {

bool equal(const std::pair<std::string, std::string>& lhs,
           const std::pair<std::string_view, std::string_view>& rhs) {
  return std::equal(lhs.first.begin(), lhs.first.end(), rhs.first.begin(),
                    rhs.first.end()) &&
         std::equal(lhs.second.begin(), lhs.second.end(), rhs.second.begin(),
                    rhs.second.end());
}

TEST(blox, map) {
  auto map = blox::map();
  EXPECT_EQ(map.end(), map.find("a"));
  map.insert(std::pair("a", "1"));
  EXPECT_NE(map.end(), map.find("a"));
  map.erase("a");
  EXPECT_EQ(map.end(), map.find("a"));
  auto input = std::vector<std::pair<std::string, std::string>>{
      {"a", "1"}, {"b", "2"}, {"c", "3"}};
  std::copy(input.begin(), input.end(), std::inserter(map, map.end()));
  auto it = map.find("b");
  ASSERT_NE(map.end(), it);
  EXPECT_EQ("2", it->second);
  EXPECT_TRUE(
      std::equal(input.begin(), input.end(), map.begin(), map.end(), equal));
}
}  // namespace test
