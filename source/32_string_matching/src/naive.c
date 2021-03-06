#include <string.h>

#include "string_matching.h"

size_t string_match_naive(char const *text, char const *comp)
{
    static size_t curr_offs = 0u;
    static size_t end_of_text = 0u;

    if (!comp) {
        curr_offs = 0u;
        end_of_text = strlen(text);
        return end_of_text;
    }

    size_t comp_len = strlen(comp);
    while (curr_offs + comp_len <= end_of_text) {
        if (strncmp(text + curr_offs, comp, comp_len) == 0) {
            size_t ret = curr_offs++;
            return ret;
        }

        ++curr_offs;
    }

    return end_of_text;
}
