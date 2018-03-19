#include <tuple>
#include <vector>

#include "gtest/gtest.h"

extern "C" {
#include "string_matching.h"
}

namespace {

static std::vector<
    std::tuple<char const *, char const *, std::vector<size_t>>> test_inputs{

    std::make_tuple("abcabaabcabac", "abcabaabcabac", std::vector<size_t>({0})),
    std::make_tuple("abcabaabcabac", "abaa", std::vector<size_t>({3})),
    std::make_tuple("abcabaabcabac", "abc", std::vector<size_t>({0, 6})),
    std::make_tuple("abcabaabcabac", "aba", std::vector<size_t>({3, 9})),
    std::make_tuple("aaaab", "aa", std::vector<size_t>({0, 1, 2})),
    std::make_tuple("aaaababa", "ba", std::vector<size_t>({4, 6})),
    std::make_tuple("abc", "ba", std::vector<size_t>({}))
};

TEST(StringMatchingTest, NaiveStringMatch) {
    for (auto const &test_input : test_inputs) {
        char const *text = std::get<0>(test_input);
        char const *comp = std::get<1>(test_input);

        std::vector<size_t> expected = std::get<2>(test_input);
        std::vector<size_t> result;

        size_t end_of_text = naive_string_match(text, nullptr);

        size_t offs;
        for (;;) {
            offs = naive_string_match(text, comp);
            if (offs == end_of_text)
                break;

            result.push_back(offs);
        }

        EXPECT_EQ(expected, result);
    }
}

}
