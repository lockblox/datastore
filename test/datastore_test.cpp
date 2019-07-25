#include <blox/datastores/lmdb.h>
#include <blox/datastores/map.h>
#include <gtest/gtest.h>
#include <cstdio>
#include <sstream>

namespace std {

bool operator==(const std::pair<std::string, std::string>& lhs,
                const std::pair<std::string_view, std::string_view>& rhs) {
  return std::equal(lhs.first.begin(), lhs.first.end(), rhs.first.begin(),
                    rhs.first.end()) &&
         std::equal(lhs.second.begin(), lhs.second.end(), rhs.second.begin(),
                    rhs.second.end());
}

bool operator==(const std::pair<std::string_view, std::string_view>& lhs,
                const std::pair<std::string, std::string>& rhs) {
  return rhs == lhs;
}

}  // namespace std

namespace test {

using lmdb_configuration = blox::datastores::lmdb::configuration;

class datastore
    : public testing::TestWithParam<std::shared_ptr<blox::datastore>> {};


TEST_P(datastore, clear) {
  auto datastore = GetParam();
  datastore->clear();
  EXPECT_TRUE(datastore->empty());
  EXPECT_EQ(0, datastore->size());
  EXPECT_LT(0, datastore->max_size());
}

TEST_P(datastore, find) {
  auto datastore = GetParam();
  EXPECT_EQ(datastore->end(), datastore->find("a"));
}

TEST_P(datastore, insert) {
  auto datastore = GetParam();
  datastore->insert(std::pair("a", "1"));
  EXPECT_NE(datastore->end(), datastore->find("a"));
}

TEST_P(datastore, erase) {
  auto datastore = GetParam();
  datastore->erase("a");
  EXPECT_EQ(datastore->end(), datastore->find("a"));
}

TEST_P(datastore, copy) {
  auto datastore = GetParam();
  auto input = std::vector<std::pair<std::string, std::string>>{
      {"a", "1"}, {"b", "2"}, {"c", "3"}};
  std::copy(input.begin(), input.end(),
            std::inserter(*datastore, datastore->end()));
  auto it = datastore->find("b");
  ASSERT_NE(datastore->end(), it);
  EXPECT_EQ("2", it->second);
  EXPECT_TRUE(std::equal(input.begin(), input.end(), datastore->begin(),
                         datastore->end()));
}

INSTANTIATE_TEST_CASE_P(
    blox, datastore,
    ::testing::Values(std::make_shared<blox::datastores::map>(),
                      std::make_shared<blox::datastores::lmdb>(
                          blox::datastores::lmdb::configuration(
                              std::filesystem::temp_directory_path()))), );

}  // namespace test
