#include <assert.h>
#include <stdlib.h>

#include "elementary_graph.h"


/* data structures */

struct adjacency_list_node {
    size_t to, edge;
    struct adjacency_list_node *next;
};

struct graph_edge_descriptor {
    size_t from, to;
};

struct graph {
    enum graph_directedness directedness;
    enum graph_representation representation;

    size_t num_vertices, num_vertices_alloced;
    size_t num_edges, num_edges_alloced;

    char *vertices_valid;
    char *edges_valid;

    union {
        struct adjacency_list_node **adjacency_lists;
        size_t **adjacency_matrix;
    };

    struct graph_edge_descriptor *edge_descriptors;
};


/* debug */

#ifndef NDEBUG

#include <stdio.h>

void graph_debug_print(struct graph *g)
{
    printf("%s graph:\n",
           (g->directedness == DIRECTED ? "directed" : "undirected"));

    printf("* %zu vertices\n", g->num_vertices);
    printf("* %zu edges:\n", g->num_edges);

    if (g->representation == ADJACENCY_LISTS) {
        for (size_t e = 0; e < g->num_edges_alloced; ++e) {
            if (!g->edges_valid[e])
                continue;

            struct graph_edge_descriptor edge = g->edge_descriptors[e];

            if (g->directedness == DIRECTED)
                printf("  %zu --%zu--> %zu\n", edge.from, e, edge.to);
            else
                printf("  %zu <--%zu--> %zu\n", edge.from, e, edge.to);
        }

        printf("* adjacency lists:\n");
        for (size_t v = 0; v < g->num_vertices; ++v) {
            printf("  %zu: [", v);

            struct adjacency_list_node *tmp = g->adjacency_lists[v];

            while (tmp) {
                printf("%zu (%zu)", tmp->to, tmp->edge);
                if (tmp->next)
                    printf(", ");

                tmp = tmp->next;
            }

            printf("]\n");
        }
    } else if (g->representation == ADJACENCY_MATRIX) {
        // TODO
    }
}

#endif


/* creation and destruction*/

struct graph * graph_create(size_t num_vertices,
                            enum graph_directedness directedness,
                            enum graph_representation representation)
{
    struct graph *g = NULL;

    if (representation == ADJACENCY_LISTS) {
        g = malloc(sizeof(*g));
        if (!g)
            return NULL;

        g->vertices_valid = malloc(num_vertices);
        if (num_vertices > 0  && !g->vertices_valid) {
            free(g);
            return NULL;
        }
        g->edges_valid = NULL;

        g->adjacency_lists = malloc(num_vertices * sizeof(*g->adjacency_lists));
        if (num_vertices > 0  && !g->adjacency_lists) {
            free(g->vertices_valid);
            free(g);
            return NULL;
        }
        g->edge_descriptors = NULL;

        g->directedness = directedness;
        g->representation = ADJACENCY_LISTS;

        g->num_vertices = num_vertices;
        g->num_vertices_alloced = num_vertices;

        g->num_edges = 0;
        g->num_edges_alloced = 0;

        for (size_t v = 0; v < num_vertices; ++v) {
            g->vertices_valid[v] = 1;
            g->adjacency_lists[v] = NULL;
        }

    } else if (representation == ADJACENCY_MATRIX) {
        // TODO
    }

    return g;
}

void graph_free(struct graph *g)
{
    if (g->representation == ADJACENCY_LISTS) {
        for (size_t v = 0; v < g->num_vertices; ++v) {
            if (!g->vertices_valid[v])
                continue;

            struct adjacency_list_node *tmp = g->adjacency_lists[v];
            struct adjacency_list_node *next;

            while (tmp) {
                next = tmp->next;
                free(tmp);
                tmp = next;
            }
        }

        free(g->adjacency_lists);
        free(g->edge_descriptors);

    } else if (g->representation == ADJACENCY_MATRIX) {
        // TODO
    }

    free(g->vertices_valid);
    free(g->edges_valid);

    free(g);
}


/* modification */

int graph_add_vertex(struct graph *g, size_t *vertex)
{
    if (g->representation == ADJACENCY_LISTS) {
        char *vertices_valid =
            realloc(g->vertices_valid, g->num_vertices_alloced + 1);

        size_t adjacency_lists_size =
            (g->num_vertices_alloced + 1) * sizeof(*g->adjacency_lists);

        struct adjacency_list_node **adjacency_lists = realloc(
            g->adjacency_lists, adjacency_lists_size);

        if (!vertices_valid || !adjacency_lists)
            return -1;

        vertices_valid[g->num_vertices_alloced] = 1;
        adjacency_lists[g->num_vertices_alloced] = NULL;

        g->vertices_valid = vertices_valid;
        g->adjacency_lists = adjacency_lists;

    } else if (g->representation == ADJACENCY_MATRIX) {
        // TODO
    }

    ++g->num_vertices;
    ++g->num_vertices_alloced;

    if (vertex)
        *vertex = g->num_vertices - 1;

    return 0;
}

int graph_add_edge(struct graph *g, size_t from, size_t to, size_t *edge)
{
    /* return if any given vertex is invalid */
    if (from >= g->num_vertices_alloced || !g->vertices_valid[from])
        return -1;

    if (to >= g->num_vertices_alloced || !g->vertices_valid[to])
        return -1;

    /* resize edge descriptor valid flag array */
    char *edges_valid = realloc(g->edges_valid, g->num_edges_alloced + 1);
    if (!edges_valid)
        return -1;

    if (g->representation == ADJACENCY_LISTS) {
        /* resize edge descriptor array */
        size_t edge_descriptors_size =
            (g->num_edges_alloced + 1) * sizeof(*g->edge_descriptors);

        struct graph_edge_descriptor *edge_descriptors = realloc(
            g->edge_descriptors, edge_descriptors_size);

        if (!edge_descriptors)
            return -1;

        /* find edge insertion position */
        struct adjacency_list_node *tmp = g->adjacency_lists[from];
        struct adjacency_list_node *last = NULL;

        while (tmp) {
            /* return if edge exists */
            if (tmp->to == to) {
                *edge = tmp->edge;
                return 0;
            }

            last = tmp;
            tmp = tmp->next;
        }

        /* allocate edge */
        struct adjacency_list_node *new_edge =
            malloc(sizeof(struct adjacency_list_node));
        if (!new_edge)
            return -1;

        new_edge->to = to;
        new_edge->edge = g->num_edges_alloced;
        new_edge->next = NULL;

        if (g->directedness == UNDIRECTED) {
            /* insert edge */
            struct adjacency_list_node *tmp_reciprocal = g->adjacency_lists[to];
            struct adjacency_list_node *last_reciprocal = NULL;

            while (tmp_reciprocal) {
                last_reciprocal = tmp_reciprocal;
                tmp_reciprocal = tmp_reciprocal->next;
            }

            struct adjacency_list_node *new_edge_reciprocal =
                malloc(sizeof(struct adjacency_list_node));
            if (!new_edge_reciprocal) {
                free(new_edge);
                return -1;
            }

            new_edge_reciprocal->to = from;
            new_edge_reciprocal->edge = g->num_edges_alloced;
            new_edge_reciprocal->next = NULL;

            if (last)
                last->next = new_edge;
            else
                g->adjacency_lists[from] = new_edge;

            if (last_reciprocal)
                last_reciprocal->next = new_edge_reciprocal;
            else
                g->adjacency_lists[to] = new_edge_reciprocal;

            /* return edge id */
            if (edge)
                *edge = g->num_edges_alloced / 2;


        } else {
            /* insert edge */
            if (last)
                last->next = new_edge;
            else
                g->adjacency_lists[from] = new_edge;

            /* return edge id */
            if (edge)
                *edge = g->num_edges_alloced;
        }

        /* insert edge into edge array */
        struct graph_edge_descriptor edge_descriptor = {from, to};
        edge_descriptors[g->num_edges_alloced] = edge_descriptor;
        g->edge_descriptors = edge_descriptors;

        /* mark edge as valid */
        edges_valid[g->num_edges_alloced] = 1;
        g->edges_valid = edges_valid;

    } else if (g->representation == ADJACENCY_MATRIX) {
        // TODO
    }

    ++g->num_edges;
    ++g->num_edges_alloced;

    return 0;
}


/* queries */

size_t graph_num_vertices(struct graph *g)
{
    return g->num_vertices;
}

size_t graph_num_edges(struct graph *g)
{
    return g->num_edges;
}

int graph_connector(struct graph *g, size_t from, size_t to, size_t *edge)
{
    /* return if any given vertex is invalid */
    if (from >= g->num_vertices_alloced || !g->vertices_valid[from])
        return -1;

    if (to >= g->num_vertices_alloced || !g->vertices_valid[to])
        return -1;

    if (g->representation == ADJACENCY_LISTS) {
        struct adjacency_list_node *tmp = g->adjacency_lists[from];

        while (tmp) {
            if (tmp->to == to) {
                *edge = tmp->edge;
                return 0;
            }

            tmp = tmp->next;
        }

        return -1;

    } else if (g->representation == ADJACENCY_MATRIX) {
        // TODO
    }
}

int graph_endpoints(struct graph *g, size_t edge, size_t *from, size_t *to)
{
    /* return if edge is invalid */
    if (edge >= g->num_edges_alloced || !g->edges_valid[edge])
        return -1;

    if (g->representation == ADJACENCY_LISTS) {
        struct graph_edge_descriptor tmp = g->edge_descriptors[edge];

        *from = tmp.from;
        *to = tmp.to;

    } else if (g->representation == ADJACENCY_MATRIX) {
        // TODO
    }

    return 0;
}

enum neighbours_incidences {
    NEIGHBOURS,
    INCIDENCES
};

static int graph_neighbours_incidences(struct graph *g, size_t vertex,
                                       enum graph_incidence incidence,
                                       size_t **ret, size_t *num_ret,
                                       enum neighbours_incidences type)
{
    *ret = NULL;
    *num_ret = 0;

    int include_ingoing = g->directedness == UNDIRECTED
                        || incidence == INGOING
                        || incidence == INGOING_AND_OUTGOING;

    int include_outgoing = g->directedness == UNDIRECTED
                         || incidence == OUTGOING
                         || incidence == INGOING_AND_OUTGOING;

    if (g->representation == ADJACENCY_LISTS) {
        // TODO: no need to iterate over all edges for undirected graph

        for (size_t e = 0; e < g->num_edges_alloced; ++e) {
            if (!g->edges_valid[e])
                continue;

            struct graph_edge_descriptor edge = g->edge_descriptors[e];

            if ((edge.from == vertex && include_outgoing)
                || (edge.to == vertex && include_ingoing)) {

                ++*num_ret;

                *ret = realloc(
                    *ret, *num_ret * sizeof(size_t));
                if (!*ret)
                    return -1;

                if (type == NEIGHBOURS) {
                    (*ret)[*num_ret - 1] =
                        edge.from == vertex ? edge.to : edge.from;
                } else {
                    (*ret)[*num_ret - 1] = e;
                }
            }
        }
    } else if (g->representation == ADJACENCY_MATRIX) {
        // TODO
    }

    return 0;
}

int graph_neighbours(struct graph *g, size_t vertex,
                     enum graph_incidence incidence,
                     size_t **neighbours, size_t *num_neighbours)
{
    return graph_neighbours_incidences(g, vertex, incidence,
                                       neighbours, num_neighbours, NEIGHBOURS);
}

int graph_incidences(struct graph *g, size_t vertex,
                     enum graph_incidence incidence,
                     size_t **incidences, size_t *num_incidences)
{
    return graph_neighbours_incidences(g, vertex, incidence,
                                       incidences, num_incidences, INCIDENCES);
}
