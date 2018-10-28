#ifndef STRING_MATCHING_H
#define STRING_MATCHING_H

size_t string_match_naive(char const *text, char const *comp);
size_t string_match_rabin_karp(char const *text, char const *comp);
size_t string_match_dfa(char const *text, char const *comp);
size_t string_match_kmp(char const *text, char const *comp);

#endif
