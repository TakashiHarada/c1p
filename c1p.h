/* c1p.h */

#ifndef __HARADALAB_KUT_C1P_H__
#define __HARADALAB_KUT_C1P_H__

#ifndef __HARADALAB_KUT_LIST_H__
#include "list.h"
#endif

#ifndef __HARADALAB_KUT_GRAPH_H__
#include "graph.h"
#endif

#include <math.h>

struct CLASS {
  int h; // head of the class
  // int l; // last of the class
  int s; // size of the class
  int n; // next class
  int p; // previous class
};
typedef struct CLASS class;

struct ELEMENT {
  int b; // which class this element belongs to
  int n; // next element
};
typedef struct ELEMENT elem;

unsigned MAX_CLASS_NUMBER;
unsigned ptr_C;

unsigned* get_c1p_order(matrix*);
class* init_class(matrix*);
elem* init_element(matrix*);
void set_p1(const adjacency_list*, const unsigned, class*, elem*);
void partition_print(const class*, const elem*);

elem* init_element(matrix* M) {
  elem* e = (elem*)calloc(M->n, sizeof(elem));
  unsigned i;
  for (i = 0; i < M->n; ++i) {
    e[i].b = -1;
    e[i].n = -1;
  }
  return e;
}

class* init_class(matrix* M) {
  class* c = (class*)calloc(M->n, sizeof(class));

  unsigned i;
  for (i = 0; i < M->n; ++i) {
    c[i].h = -1;
    c[i].s = -1;
    c[i].n = -1;
    c[i].p = -1;
  }

  ptr_C = 0;
  MAX_CLASS_NUMBER = 0;
  
  return c;
}

void set_p1(const adjacency_list* M, const unsigned i, class* c, elem* e) {
  list_unsigned_cell* p;
  unsigned j, k;
  p = M->r[i]->head;
  k = p->key;
  c[ptr_C].h = k;
  e[k].b = ptr_C;
  for (p = p->next; NULL != p; p = p->next) {
    j = k;
    k = p->key;
    e[j].n = k;
    e[k].b = ptr_C;
    /* printf("%d \n", p->key); */
  }
}

unsigned* get_c1p_order(matrix* M) {

  graph* G = make_overlap_graph(M);
  decomposing_to_connected_components(G);
  graph_print(G);

  class* c = init_class(M);
  elem* e = init_element(M);
  
  adjacency_list* Ma = matrix2adjacency_list(M);
  /* adjacency_list_print(Ma); */
					     
  unsigned* order = (unsigned*)calloc(M->n, sizeof(unsigned));

  unsigned num_c = G->num_of_components;
  printf("#component = %d\n", num_c);

  
  
  unsigned k;
  for (k = 0; k < num_c; ++k) {
    unsigned t = G->com[k]->size;
    if (1 == t)
      continue;
    list_unsigned* L = spanning_tree(G, k);
    /* printf("ST[%d] : ", k); list_unsigned_print(L); printf("\n"); */

    list_unsigned_cell* p = L->head;
    set_p1(Ma, p->key, c, e);
    MAX_CLASS_NUMBER = 1;
    partition_print(c, e);

    return NULL;
    
  /*   for (p = p->next; NULL != p; p = p->next) { */
  /*     if (1 < r[p->key]->size) { */
  /*   	lr lr = refine(P[k], Col, r[p->key]); */
  /*   	if (!is_consecutive(lr)) { */
  /*   	  /\* printf("Input Matrix has non-C1P\n"); *\/ */
  /* 	  partitions_clear(P, num_c); */
  /* 	  list_rows_clear(r, M->m); */
  /* 	  columns_clear(Col); */
  /* 	  graph_clear(G); */
  /* 	  return NULL; */
  /*   	} */
  /*     } */
  /*     reset_do_intersect_P(P[k]); */
  /*     reset_do_intersect_rows(r[p->key]); */
  /*     reset_class_counter(P[k]); */
  /*   } */
    list_unsigned_clear(L); L = NULL;
  /*   reset_cls_pointer(Col); */
  }
  adjacency_list_clear(Ma); Ma = NULL;
  graph_clear(G); G = NULL;
  
  return order;
}


void partition_print(const class* c, const elem* e) {
  int i = ptr_C;
  while (-1 != i) {
    int j = c[i].h;
    printf("{ %d", j);
    j = e[j].n;
    while (-1 != j) {
      printf(", %d", j);
      j = e[j].n;
    }
    printf(" }\n");
    i = c[i].n;
  }
}

void order_print(const unsigned*, const unsigned);
bool check_strongC1P_matrix(matrix*);

void order_print(const unsigned* s, const unsigned n) {
  unsigned i;
  printf("%d", s[0]);
  for (i = 1; i < n; ++i)
    printf(", %d", s[i]);
  putchar('\n');
}

bool check_strongC1P_matrix(matrix* M) {
  unsigned i;
  for (i = 0; i < M->m; ++i) {
    unsigned j;
    /* state = 0 means that M[i][0] .. M[i][j-1] are '0'
     * state = 1 means that for some k, M[i][k] .. M[i][j-1] are '1' */
    unsigned state = 0;
    for (j = 0; j < M->n; ++j) {
      if ('1' == M->b[i][j] && 0 == state) { ++state; }
      else if ('0' == M->b[i][j] && 1 == state) { ++state; }
      else if ('1' == M->b[i][j] && 2 == state) { return false; }
      /* printf("i = %d, j = %d, state = %d\n", i, j, state); */
    }
  }
  return true;
}

#endif
