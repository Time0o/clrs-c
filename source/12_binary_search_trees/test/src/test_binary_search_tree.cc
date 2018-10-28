#include <algorithm>
#include <cstdlib>
#include <vector>

#include "gtest/gtest.h"

extern "C" {
#include "binary_search_tree.h"
}

using ::testing::TestWithParam;
using ::testing::Values;


TEST(EmptyBinarySearchTreeTest, CanOperateOnEmptyTree)
{
    EXPECT_EQ(bst_search(nullptr, nullptr, nullptr), nullptr)
        << "Searching for key in empty BST yield null pointer.";

    EXPECT_EQ(bst_min(nullptr), nullptr)
        << "Searching for minimum in empty BST yield null pointer.";

    EXPECT_EQ(bst_max(nullptr), nullptr)
        << "Searching for maximum in empty BST yield null pointer.";

    EXPECT_EQ(bst_predecessor(nullptr), nullptr)
        << "Searching for predecessor of invalid BST node yiels null pointer.";

    EXPECT_EQ(bst_predecessor(nullptr), nullptr)
        << "Searching for successor of invalid BST node yiels null pointer.";

    auto it = bst_iter_create(nullptr);

    EXPECT_FALSE(bst_iter_has_next(it))
        << "Empty BST iterator has no next node.";

    EXPECT_EQ(bst_iter_next(it), nullptr)
        << "Empty BST iterator yields null pointer.";

    bst_iter_free(it);
}

class BinarySearchTreeTest : public TestWithParam<std::vector<int>>
{
enum { DATA_BLOCK_SIZE = 1024 };

protected:
    void SetUp() {
        for (int i : GetParam()) {
            int *i_ptr = static_cast<int *>(malloc(sizeof(int)));
            *i_ptr = i;

            void *data = malloc(DATA_BLOCK_SIZE);

            bst_root = bst_insert(bst_root, i_ptr, data, intcomp);
        }
    }

    void TearDown() {
        bst_free(bst_root, 1, 1);
    }

    struct bst_node *bst_root = nullptr;

    static int intcomp(void const *lhs_ptr, void const *rhs_ptr) {
        int lhs = *static_cast<int const *>(lhs_ptr);
        int rhs = *static_cast<int const *>(rhs_ptr);

        if (lhs < rhs)
            return -1;
        else if (lhs > rhs)
            return 1;
        else
            return 0;
    }
};

INSTANTIATE_TEST_CASE_P(BinarySearchTrees, BinarySearchTreeTest, Values(
    std::vector<int>({3, 4, 2, 2, 3}),
    std::vector<int>({1, 3, 0, 3, 2}),
    std::vector<int>({5, 5, 0, 5, 2}),
    std::vector<int>({1, 3, 0, 3, 1}),
    std::vector<int>({2, 7, 5, 10, 1, 5, 7, 1, 8, 0}),
    std::vector<int>({8, 10, 3, 2, 6, 10, 10, 10, 7, 8}),
    std::vector<int>({2, 6, 5, 3, 4, 6, 3, 9, 4, 2}),
    std::vector<int>({10, 8, 2, 8, 4, 5, 10, 9, 9, 8}),
    std::vector<int>({9, 4, 10, 13, 10, 4, 1, 3, 8, 4, 1, 13, 10, 7, 15}),
    std::vector<int>({7, 10, 10, 4, 11, 7, 6, 12, 14, 12, 5, 5, 4, 11, 7}),
    std::vector<int>({13, 0, 0, 11, 10, 0, 0, 13, 8, 0, 5, 5, 1, 2, 4}),
    std::vector<int>({3, 15, 9, 1, 5, 14, 3, 14, 10, 15, 0, 9, 14, 14, 11})));

TEST_P(BinarySearchTreeTest, CanDeleteFromTree)
{
    auto vect = GetParam();

    for (auto it = vect.begin(); it != vect.end(); ++it) {
        int key = *it;

        auto node = bst_search(bst_root, &key, intcomp);
        ASSERT_NE(node, nullptr)
            << "BST node to delete found.";

        bst_root = bst_delete(bst_root, node, 1, 1);

        std::vector<int> remaining(it + 1, vect.end());
        std::sort(remaining.begin(), remaining.end());

        auto bst_it = bst_iter_create(bst_root);

        for (auto key_remaining : remaining) {
            ASSERT_TRUE(bst_iter_has_next(bst_it))
                << "BST iterator has next node.";

            struct bst_node *next = bst_iter_next(bst_it);
            ASSERT_NE(next, nullptr)
                << "BST iterator's next node is valid.";

            ASSERT_EQ(*static_cast<int *>(next->key), key_remaining)
                << "BST iterator's returns correct next nodes for reducted BST.";
        }

        bst_iter_free(bst_it);
    }
}

TEST_P(BinarySearchTreeTest, CanSearchTree)
{
    auto expected = GetParam();

    for (auto key : expected) {
        auto node = bst_search(bst_root, &key, intcomp);
        ASSERT_NE(node, nullptr)
            << "BST search returns valid node.";

        ASSERT_EQ(*static_cast<int *>(node->key), key)
            << "BST search returns correct node.";

        auto num_duplicates =
            std::count(expected.begin(), expected.end(), key) - 1u;

        for (auto i = 0u; i < num_duplicates; ++i) {
            ASSERT_NE(node->right, nullptr)
                << "BST node with further key occurrences has right child node.";

            auto subnode = bst_search(bst_root,  &key, intcomp);
            ASSERT_TRUE(subnode != nullptr
                        && *static_cast<int *>(subnode->key) == key)
                << "Next BST key occurrence found in node's right subtree.";

            node = subnode;
        }
    }
}

TEST_P(BinarySearchTreeTest, CanFindTreeMinMax)
{
    auto expected = GetParam();

    auto p = std::minmax_element(expected.begin(), expected.end());
    int min = *p.first;
    int max = *p.second;

    auto min_node = bst_min(bst_root);
    ASSERT_NE(min_node, nullptr)
        << "BST minimum node found.";

    EXPECT_EQ(*static_cast<int *>(min_node->key), min)
        << "BST minimum node has correct key.";

    auto max_node = bst_max(bst_root);
    ASSERT_NE(max_node, nullptr)
        << "BST maximum node found.";

    EXPECT_EQ(*static_cast<int *>(max_node->key), max)
        << "BST maximum node has correct key.";
}

TEST_P(BinarySearchTreeTest, CanFindTreePredSucc)
{
    auto expected = GetParam();
    std::sort(expected.begin(), expected.end());

    std::map<int, std::size_t> multiplicities;

    int last = expected[0];
    multiplicities[last] = 1u;

    for (auto i = 1u; i < expected.size(); ++i) {
        int next = expected[i];

        if (next == last) {
           ++multiplicities[last];
        } else {
            multiplicities[next] = 1u;
            last = next;
        }
    }

    auto it = std::unique(expected.begin(), expected.end());
    expected.erase(it, expected.end());

    for (auto i = 0u; i < expected.size(); ++i) {
        int key = expected[i];
        std::size_t multiplicity = multiplicities[key];

        auto node = bst_search(bst_root, &key, intcomp);
        ASSERT_TRUE(node != nullptr
                    && *static_cast<int *>(node->key) == key)
            << "BST node with key " << key << " exists.";

        auto pred = bst_predecessor(node);

        if (i > 0u) {
            ASSERT_NE(pred, nullptr)
                << "BST node (key " << key << ") predecessor exists.";

            EXPECT_EQ(*static_cast<int *>(pred->key), expected[i - 1u])
                << "BST node (key " << key << ") predecessor has correct key.";
        } else {
            EXPECT_EQ(pred, nullptr)
                << "BST minimum node has no predecessor.";
        }

        auto succ = bst_successor(node);

        for (auto i = 1u; i < multiplicity; ++i) {
            ASSERT_NE(succ, nullptr)
                << "BST node (key " << key << ") successor with same key exists.";

            EXPECT_EQ(*static_cast<int *>(succ->key), key)
                << "BST node (key " << key << ") successor with same key has correct key.";

            succ = bst_successor(succ);
        }

        if (i < expected.size() - 1u) {
            ASSERT_NE(succ, nullptr)
                << "BST node (key " << key << ") successor with different key exists.";

            EXPECT_EQ(*static_cast<int *>(succ->key), expected[i + 1u])
                << "BST node (key " << key << ") successor with different key has correct key.";
        } else {
            EXPECT_EQ(succ, nullptr)
                << "BST maximum node has no successor.";
        }
    }
}

TEST_P(BinarySearchTreeTest, CanIterateTree)
{
    auto expected = GetParam();
    std::sort(expected.begin(), expected.end());

    auto it = bst_iter_create(bst_root);
    for (auto i = 0u; i < expected.size(); ++i) {
        ASSERT_TRUE(bst_iter_has_next(it))
            << "BST iterator has next node (" << i << ").";

        struct bst_node *next = bst_iter_next(it);
        ASSERT_NE(next, nullptr)
            << "BST iterator's next node (" << i << ") is valid.";

        ASSERT_EQ(*static_cast<int *>(next->key), expected[i])
            << "BST iterator's next node (" << i << ") has correct key.";
    }

    EXPECT_EQ(bst_iter_next(it), nullptr)
        << "Exhausted BST iterator yield null pointer.";

    bst_iter_free(it);
}
