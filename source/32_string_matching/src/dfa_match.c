#include <stdlib.h>
#include <string.h>

#include "string_matching.h"

#define HASHSIZE 10

struct node {
    char input;
    size_t next_state;
    struct node *next;
};

struct transitions {
    size_t states;
    struct node ***table;
};

struct transitions * alloc_transitions(size_t states)
{
    struct transitions *ret = malloc(sizeof(struct transitions));
    ret->states = states;
    ret->table = malloc(states * sizeof(struct node **));
    for (size_t i = 0; i < states; ++i) {
        ret->table[i] = malloc(HASHSIZE * sizeof(struct node *));
        for (size_t j = 0; j < HASHSIZE; ++j)
            ret->table[i][j] = NULL;
    }

    return ret;
};

static void add_transition(
    struct transitions *delta, int state, char input, int next_state)
{
    struct node *tmp = malloc(sizeof(struct node));
    tmp->input = input;
    tmp->next_state = next_state;
    tmp->next = delta->table[state][(unsigned char) input % HASHSIZE];
    delta->table[state][(unsigned char) input % HASHSIZE] = tmp;
}

static void free_transitions(struct transitions *delta)
{
    for (size_t i = 0; i < delta->states; ++i) {
        for (size_t j = 0; j < HASHSIZE; ++j) {

            struct node *list = delta->table[i][j];
            while (list) {
                struct node *tmp = list;
                list = list->next;
                free(tmp);
            }
        }

        free(delta->table[i]);
    }

    free(delta->table);
    free(delta);
}

static struct transitions * compute_transitions(
    char const *pattern, size_t pattern_len)
{
    /* note that this function is cubic in len(pattern) und far from optimal */

    static int charset[256];
    static char sigma[256]; /* all distinct characters in input pattern */

    /* determine sigma */
    size_t n_sigma = 0;
    for (size_t i = 0; i < pattern_len; ++i) {
        char c = pattern[i];
        if (!charset[(unsigned char) c]) {
            sigma[n_sigma++] = c;
            charset[(unsigned char) c] = 1;
        }
    }

    for (size_t s = 0; s < n_sigma; ++s)
        charset[(unsigned char) sigma[s]] = 0;

    /* compute delta */
    struct transitions *delta = alloc_transitions(pattern_len + 1);

    for (size_t q = 0; q <= pattern_len; ++q) {
        for (size_t s = 0; s < n_sigma; ++s) {
            size_t pref = pattern_len < q + 1 ? pattern_len : q + 1;

            do {
                if (pattern[pref - 1] != sigma[s])
                    continue;

                if (q == 0 ||
                    strncmp(pattern, pattern + q + 1 - pref, pref - 1) == 0) {

                    add_transition(delta, q, sigma[s], pref);
                    break;
                }

            } while (--pref > 0);
        }
    }

    return delta;
}

size_t dfa_match(char const *text, char const *pattern)
{
    static int init = 0;

    static size_t end_of_text = 0;
    static size_t pattern_len = 0;
    static size_t text_offs = 0;

    static struct transitions *delta = NULL;

    if (!pattern) {
        init = 1;

        end_of_text = strlen(text);
        return end_of_text;
    }

    if (init) {
        init = 0;

        pattern_len = strlen(pattern);
        text_offs = 0;

        delta = compute_transitions(pattern, pattern_len);
    }

    size_t state = 0;
    while (text_offs < end_of_text) {
        char input = text[text_offs];
        struct node *l = delta->table[state][(unsigned char) input % HASHSIZE];

        state = 0;
        while (l) {
            if (l->input == input) {
                state = l->next_state;
                break;
            }
            l = l->next;
        }

        if (state == pattern_len)
            return text_offs + 1 - pattern_len;

        ++text_offs;
    }

    free_transitions(delta);

    return end_of_text;
}
