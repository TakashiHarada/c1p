/* graph.h */

#ifndef __HARADALAB_KUT_GRAPH_H__
#define __HARADALAB_KUT_GRAPH_H__

#ifndef __HARADALAB_KUT_LIST_H__
#include "list.h"
#endif

#ifndef __HARADALAB_KUT_SET_H__
#include "set.h"
#endif

#ifndef __HARADALAB_KUT_MATRIX_H__
#include "matrix.h"
#endif

#include <math.h>

struct GRAPH {
  unsigned order;             /* number of vertices */
  list_unsigned** al;         /* adjacency list representing edges */
  unsigned num_of_components; /* number of connected components */
  int *cn;                    /* connected component number (id) for each vertex */
  list_unsigned** com;
};
typedef struct GRAPH graph;

void graph_print(graph*);
void graph_clear(graph*);
graph* make_overlap_graph(matrix*);
void dfs(graph*, unsigned, int);
void decomposing_to_connected_components(graph*);
void dfs_span_tree(graph*, unsigned, list_unsigned*, unsigned, unsigned, int*);
list_unsigned* spanning_tree(graph*, unsigned);
  
void graph_print(graph* G) {
  const unsigned d = floor(log10(G->order)) + 1;
  unsigned i;
  for (i = 0; i < G->order; ++i) {
    printf("al[%*d] : ", d, i);
    list_unsigned_print(G->al[i]); putchar('\n');
  }
  if (NULL != G->com)
    for (i = 0; i < G->num_of_components; ++i) {
      printf("cn[%*d] : ", d, i);
      list_unsigned_print(G->com[i]);
      putchar('\n'); }
}

void graph_clear(graph* G) {
  unsigned i;
  for (i = 0; i < G->order; ++i) { list_unsigned_clear(G->al[i]); }
  free(G->al);
  if (NULL != G->cn) { free(G->cn); }
  if (NULL != G->com) {
    for (i = 0; i < G->num_of_components; ++i) { list_unsigned_clear(G->com[i]); }
    free(G->com);
  }
  free(G);
}

graph* make_overlap_graph(matrix* M) {
  graph* G = (graph*)calloc(1,sizeof(graph));
  G->order = M->m; /* the number of vertices (G->order) is M->m */
  G->al = (list_unsigned**)calloc(G->order, sizeof(list_unsigned*));

  unsigned i;
  for (i = 0; i < G->order; ++i)
    G->al[i] = (list_unsigned*)calloc(1, sizeof(list_unsigned));

  set_unsigned** r = (set_unsigned**)calloc(G->order, sizeof(set_unsigned*));
  for (i = 0; i < G->order; ++i) {
    r[i] = (set_unsigned*)calloc(1, sizeof(set_unsigned));
    set_unsigned_init(r[i]);
  }

  for (i = 0; i < M->m; ++i) {
    unsigned j;
    for (j = 0; j < M->n; ++j)
      if ('1' == M->b[i][j])
	set_unsigned_insert(r[i], j);
  }

  for (i = 0; i < G->order; ++i) {
    unsigned j;
    for (j = i+1; j < G->order; ++j) {
      set_unsigned* R = set_unsigned_intersect(r[i], r[j]);
      if (0 != R->size && R->size != r[i]->size && R->size != r[j]->size) {
	list_unsigned_add_rear(G->al[i], j);
	list_unsigned_add_rear(G->al[j], i);
      }
      set_unsigned_clear(R); R = NULL;
    }
  }
  
  /* free the dynamically allocated area */
  for (i = 0; i < G->order; ++i)
    set_unsigned_clear(r[i]);
  free(r);
  
  return G;
}

void dfs(graph* G, unsigned v, int k) {
  G->cn[v] = k;
  list_unsigned_cell* p;
  for (p = G->al[v]->head; NULL != p; p = p->next) { if (-1 == G->cn[p->key]) { dfs(G, p->key, k); } }
}

void decomposing_to_connected_components(graph* G) {
  G->cn = (int*)calloc(G->order, sizeof(unsigned));

  unsigned v;
  for (v = 0; v < G->order; ++v)
    G->cn[v] = -1;

  int k = 0;
  for (v = 0; v < G->order; ++v)
    if (-1 == G->cn[v]) {
      dfs(G, v, k); ++k;
    }

  G->num_of_components = k;
  G->com = (list_unsigned**)calloc(G->num_of_components, sizeof(list_unsigned*));
  for (v = 0; v < G->num_of_components; ++v)
    G->com[v] = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  for (v = 0; v < G->order; ++v)
    list_unsigned_add_rear(G->com[G->cn[v]], v);
}

void dfs_span_tree(graph* G, unsigned C, list_unsigned* L, unsigned v, unsigned k, int* cn) {
  cn[v] = k;
  /* printf("v = %d\n", v); */
  list_unsigned_add_rear(L, v);
  list_unsigned_cell* p;
  for (p = G->al[v]->head; NULL != p; p = p->next) { if (-1 == cn[p->key]) { dfs_span_tree(G, C, L, p->key, k, cn); } }
}

/* C is a component number */
list_unsigned* spanning_tree(graph* G, unsigned C) {
  list_unsigned* L = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  L->head = L->last = NULL;
  int* cn = (int*)calloc(G->order, sizeof(int));
  list_unsigned_cell* p;
  for (p = G->com[C]->head; NULL != p; p = p->next) { cn[p->key] = -1; }
  unsigned k = 0;
  for (p = G->com[C]->head; NULL != p; p = p->next) { if (-1 == cn[p->key]) { dfs_span_tree(G, C, L, p->key, k, cn); ++k; } }
  /* for (p = G->com[C]->head; NULL != p; p = p->next) { if (0 == cn[p->key]) { dfs_span_tree(G, C, L, p->key, k, cn); ++k; } } */

  free(cn); cn = NULL;
  return L;
}

#endif
