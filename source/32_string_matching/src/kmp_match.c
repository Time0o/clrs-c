#include <stdlib.h>
#include <string.h>

#include "string_matching.h"

static size_t * compute_prefixes(char const *pattern, size_t pattern_len)
{
    size_t *prefixes = malloc(pattern_len * sizeof(size_t));

    prefixes[0] = 0;
    size_t matching = 0;
    for (size_t state = 1; state < pattern_len; ++state) {
        while (matching > 0 && pattern[matching] != pattern[state])
            matching = prefixes[matching];
        if (pattern[matching] == pattern[state])
            ++matching;
        prefixes[state] = matching;
    }

    return prefixes;
}

size_t kmp_match(char const *text, char const *pattern)
{
    static int init = 0;

    static size_t end_of_text = 0;
    static size_t pattern_len = 0;
    static size_t text_offs = 0;
    static size_t state = 0;

    static size_t *prefixes = NULL;

    if (!pattern) {
        init = 1;

        end_of_text = strlen(text);
        return end_of_text;
    }

    if (init) {
        init = 0;

        pattern_len = strlen(pattern);
        text_offs = 0;

        prefixes = compute_prefixes(pattern, pattern_len);

        state = 0;
    }

    while (text_offs < end_of_text) {
        while (state > 0 && pattern[state] != text[text_offs])
            state = prefixes[state - 1];
        if (pattern[state] == text[text_offs])
            ++state;
        if (state == pattern_len) {
            state = prefixes[state - 1];
            return ++text_offs - pattern_len;
        }
        ++text_offs;
    }

    free(prefixes);
    return end_of_text;
}
