#include <functional>
#include <tuple>
#include <vector>

#include "gtest/gtest.h"

extern "C" {
#include "string_matching.h"
}

using ::testing::TestWithParam;
using ::testing::Values;


class StringMatchTest
    : public TestWithParam<std::function<std::size_t(char const*, char const*)>>
{
using test_input_type =
    std::tuple<char const *, char const *, std::vector<size_t>>;

protected:
    std::vector<test_input_type> test_inputs {
        std::make_tuple("abcabaabcabac", "abcabaabcabac",
                        std::vector<std::size_t>({0})),
        std::make_tuple("abcabaabcabac", "abaa",
                        std::vector<std::size_t>({3})),
        std::make_tuple("abcabaabcabac", "abc",
                        std::vector<std::size_t>({0, 6})),
        std::make_tuple("abcabaabcabac", "aba",
                        std::vector<std::size_t>({3, 9})),
        std::make_tuple("aaaab", "aa",
                        std::vector<std::size_t>({0, 1, 2})),
        std::make_tuple("aaaababa", "ba",
                        std::vector<std::size_t>({4, 6})),
        std::make_tuple("abc", "ba",
                        std::vector<std::size_t>({}))
    };
};

TEST_P(StringMatchTest, CanMatchPatterns)
{
    auto match = GetParam();

    for (auto const &test_input : test_inputs) {
        char const *text = std::get<0>(test_input);
        char const *comp = std::get<1>(test_input);

        std::vector<size_t> expected = std::get<2>(test_input);
        std::vector<size_t> result;

        std::size_t end_of_text = match(text, nullptr);

        for (std::size_t i = 0u; i < expected.size(); ++i)
            result.push_back(match(text, comp));

        EXPECT_EQ(end_of_text, match(text, comp))
            << "string matching algorithm returns end of text after all matches have been found";

        EXPECT_EQ(expected, result)
            << "string matching algorithm produces correct output";
    }
}

INSTANTIATE_TEST_CASE_P(StringMatchAlgorithms, StringMatchTest, Values(
    string_match_naive,
    string_match_rabin_karp,
    string_match_dfa,
    string_match_kmp));
