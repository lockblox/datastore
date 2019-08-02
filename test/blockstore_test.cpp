#include <blox/blockstore.h>
#include <blox/datastores/map.h>
#include <gtest/gtest.h>

namespace test {

class blockstore
    : public testing::TestWithParam<std::shared_ptr<blox::blockstore>> {};

TEST_P(blockstore, clear) {
  auto blockstore = GetParam();
  EXPECT_NO_THROW(blockstore->clear());
}

TEST_P(blockstore, empty) {
  auto blockstore = GetParam();
  EXPECT_TRUE(blockstore->empty());
}

TEST_P(blockstore, size) {
  auto blockstore = GetParam();
  EXPECT_EQ(0, blockstore->size());
}

TEST_P(blockstore, max_size) {
  auto blockstore = GetParam();
  EXPECT_LT(0, blockstore->max_size());
}

TEST_P(blockstore, insert) {
  auto blockstore = GetParam();
  auto block = blox::block("my data");
  blockstore->insert(block);
  EXPECT_EQ(1, blockstore->size());
  EXPECT_FALSE(blockstore->empty());
  EXPECT_NE(blockstore->end(), blockstore->find(block));
  EXPECT_EQ(1, blockstore->erase(block));
  EXPECT_EQ(blockstore->end(), blockstore->find(block));
  EXPECT_TRUE(blockstore->empty());
  EXPECT_EQ(0, blockstore->size());
}

TEST_P(blockstore, iteration) {
  auto blockstore = GetParam();
  blockstore->clear();
  auto input = std::set<blox::block>{blox::block{"a"}, blox::block{"b"},
                                     blox::block{"c"}};
  std::copy(input.begin(), input.end(),
            std::inserter(*blockstore, blockstore->end()));
  EXPECT_EQ(input.size(), blockstore->size());
  EXPECT_TRUE(std::equal(input.begin(), input.end(), blockstore->begin(),
                         blockstore->end()));
}

INSTANTIATE_TEST_CASE_P(blox, blockstore,
                        ::testing::Values(std::make_shared<blox::blockstore>(
                            blox::datastores::make_map())), );

}  // namespace test