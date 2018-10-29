#ifndef ELEMENTARY_GRAPH_H
#define ELEMENTARY_GRAPH_H

/* constants */

enum graph_directedness {
    DIRECTED,
    UNDIRECTED
};

enum graph_representation {
    ADJACENCY_LISTS,
    ADJACENCY_MATRIX
};

enum graph_incidence {
    INGOING,
    OUTGOING,
    INGOING_AND_OUTGOING
};


/* opaque data structure */

struct graph;


/* creation and destruction*/

struct graph * graph_create(size_t num_vertices,
                            enum graph_directedness directedness,
                            enum graph_representation representation);

void graph_free(struct graph *g);


/* modification */

int graph_add_vertex(struct graph *g, size_t *vertex);
int graph_remove_vertex(struct graph *g, size_t vertex);

int graph_add_edge(struct graph *g, size_t from, size_t to, size_t *edge);
int graph_remove_edge(struct graph *g, size_t edge);


/* queries */

size_t graph_num_vertices(struct graph *g);
size_t graph_num_edges(struct graph *g);

int graph_connector(struct graph *g, size_t from, size_t to, size_t *edge);

int graph_endpoints(struct graph *g, size_t edge, size_t *from, size_t *to);

int graph_neighbours(struct graph *g, size_t vertex,
                     enum graph_incidence incidence,
                     size_t **neighbours, size_t *num_neighbours);

int graph_incidences(struct graph *g, size_t vertex,
                     enum graph_incidence incidence,
                     size_t **incidences, size_t *num_incidences);

int graph_degree(struct graph *g, size_t vertex, size_t *degree);


/* vertex iteration */

struct graph_vertex_iter;

struct graph_vertex_iter * graph_vertex_iter_create(struct graph *g, size_t vertex);
void graph_vertex_iter_free(struct graph_vertex_iter *it);

int graph_vertex_iter_has_next(struct graph_vertex_iter const *it);
size_t graph_vertex_iter_next(struct graph_vertex_iter *it);


/* edge iteration */

struct graph_edge_iter;

struct graph_edge_iter * graph_edge_iter_create(struct graph *g, size_t edge);
void graph_edge_iter_free(struct graph_edge_iter *it);

int graph_edge_iter_has_next(struct graph_edge_iter const *it);
size_t graph_edge_iter_next(struct graph_edge_iter *it);

#endif
