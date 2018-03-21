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

static void test_string_match(size_t (*match)(char const *, char const *))
{
    for (auto const &test_input : test_inputs) {
        char const *text = std::get<0>(test_input);
        char const *comp = std::get<1>(test_input);

        std::vector<size_t> expected = std::get<2>(test_input);
        std::vector<size_t> result;

        size_t end_of_text = match(text, nullptr);

        size_t offs;
        for (size_t i = 0u; i < expected.size(); ++i) {
            offs = match(text, comp);
            if (offs == end_of_text)
                break;

            result.push_back(offs);
        }

        EXPECT_EQ(expected, result)
            << "string matching algorithm produces correct output";
    }
}

TEST(StringMatchingTest, NaiveStringMatch) {
    test_string_match(naive_string_match);
}

TEST(StringMatchingTest, RabinKarpMatch) {
    test_string_match(rabin_karp_match);
}

TEST(StringMatchingTest, DFAMatch) {
    test_string_match(dfa_match);
}

TEST(StringMatchingTest, KMPMatch) {
    test_string_match(kmp_match);
}

}
