#ifndef STRING_MATCHING_H
#define STRING_MATCHING_H

size_t naive_string_match(char const *text, char const *comp);
size_t rabin_karp_match(char const *text, char const *comp);
size_t dfa_match(char const *text, char const *comp);
size_t kmp_match(char const *text, char const *comp);

#endif
