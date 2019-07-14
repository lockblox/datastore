#include <blox/block.h>
#include <gtest/gtest.h>

namespace test {

TEST(blocks, equal) {
    auto a = blox::block("some data");
    auto b = blox::block("some other data");
    EXPECT_NE(a, b);
    b = blox::block(a.key(), b.data()); // make keys match
    EXPECT_EQ(a,b);
    b = blox::block(a.key()); // no data in block b
    EXPECT_EQ(a,b);
    auto a_copy = a;
    EXPECT_EQ(a_copy.key(), a.key());
    EXPECT_EQ(a_copy.data(), a.data());
    EXPECT_EQ(a, b);
    a = a;
    EXPECT_EQ(a,a);
    blox::block c{b};
    EXPECT_EQ(b, c);
    auto a_move = std::move(a);
    EXPECT_EQ(a_move, a_copy);
}

TEST(blocks, set) {
  auto block_set = std::set<blox::block>{blox::block("a"), blox::block("b"),
                                         blox::block("c"), blox::block("d"),
                                         blox::block("a"), blox::block("b")};
  EXPECT_EQ(4u, block_set.size());
}

}