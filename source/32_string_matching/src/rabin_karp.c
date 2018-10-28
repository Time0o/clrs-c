#include <string.h>

#include "string_matching.h"

/* a prime such that 256*Q is significantly smaller than the least possible
   maximum values a long can take on. */
#define Q 251

size_t string_match_rabin_karp(char const *text, char const *comp)
{
    static int init = 0;

    static size_t end_of_text = 0u;
    static size_t comp_len = 0u;
    static size_t shift = 0u;

    static long msd = 1;
    static long text_val = 0;
    static long comp_val = 0;

    if (!comp) {
        init = 1;

        end_of_text = strlen(text);
        return end_of_text;
    }

    if (init) {
        init = 0;

        comp_len = strlen(comp);

        msd = 1;
        text_val = 0;
        comp_val = 0;
        for (size_t i = 0u; i < comp_len; ++i) {
            if (i != comp_len - 1)
                msd = (msd << 8) % Q;

            text_val = ((text_val << 8) + text[i]) % Q;
            comp_val = ((comp_val << 8) + comp[i]) % Q;
        }

        shift = 0u;
    }

    while (shift <= end_of_text - comp_len) {
        size_t ret = end_of_text;

        if (comp_val == text_val && strncmp(text + shift, comp, comp_len) == 0)
            ret = shift;

        if (shift != end_of_text - comp_len) {
            text_val = (((text_val - text[shift] * msd) << 8) +
                        text[shift + comp_len]) % Q;
            if (text_val < 0)
                text_val += Q;
        }

        ++shift;

        if (ret != end_of_text)
            return ret;
    }

    return end_of_text;
}
