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
  int t; // tail of the class
  int s; // size of the class
  int n; // next class
  int p; // previous class
};
typedef struct CLASS class;

unsigned MAX_CLASS_NUMBER;
unsigned cls_ptr_h;
unsigned cls_ptr_t;


struct ELEMENT {
  int b; // which class this element belongs to
  int n; // next element
  int p; // previous element
};
typedef struct ELEMENT elem;

unsigned elem_ptr_h;

void element_insert(elem*, const int, const int);
void element_remove(elem*, const int);
void element_print(const elem*, const int);

// insert 'i' next to the e[ptr]
// It is possible to i become to -1.
void element_insert(elem* e, const int ptr, const int i) {
  if (-1 == ptr) {
    e[i].n = elem_ptr_h;
    e[i].p = -1;
    e[elem_ptr_h].p = i;
    elem_ptr_h = i;
  }
  else {
    e[i].n = e[ptr].n;
    e[i].p = ptr;
    e[ptr].n = i;
    if (-1 != e[i].n)
      e[e[i].n].p = i;
  }
}

void element_remove(elem* e, const int i) {
  if (-1 != e[i].p)
    e[e[i].p].n = e[i].n;
  if (-1 != e[i].n)
    e[e[i].n].p = e[i].p;
  if (-1 == e[i].p)
    elem_ptr_h = e[i].n;
  e[i].b = -1;
  e[i].n = -1;
  e[i].p = -1;
}

void element_print(const elem* e, const int i) {
  printf("%d", i);
  int j = e[i].n;
  for ( ; -1 != j; j = e[j].n)
    printf(" --> %d", j);
  putchar('\n');
}


struct LEFT_RIGHT {
  unsigned l;
  unsigned r;
};
typedef struct LEFT_RIGHT lr;


unsigned* get_c1p_order(matrix*);
class* init_class(matrix*);
elem* init_element(matrix*);
void set_p0(const adjacency_list*, const unsigned, class*, elem*);
lr refine(class*, elem*, const list_unsigned*);
list_unsigned* intersection_SandT(elem*, const list_unsigned*);
list_unsigned* intersection_piAndT(elem*, const int, const list_unsigned*);
unsigned check_cases(const list_unsigned*, const list_unsigned*);
lr refine_sub(class*, elem*, const list_unsigned*, const list_unsigned*, const unsigned);
void insert_new_class(class*, elem*, const list_unsigned*, const unsigned);
void refine_2(class*, elem*, const list_unsigned*);
bool refine_2_a(class*, elem*, const list_unsigned*);
void partition_print(const class*, const elem*);


elem* init_element(matrix* M) {
  elem* e = (elem*)calloc(M->n, sizeof(elem));
  int i;
  for (i = 0; (unsigned)i < M->n; ++i) {
    e[i].b = -1;
    e[i].p = i-1;
    e[i].n = i+1;
  }
  e[M->n-1].n = -1;
  elem_ptr_h = 0;
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

  cls_ptr_h = cls_ptr_t = 0;
  MAX_CLASS_NUMBER = 0;
  
  return c;
}

void set_p0(const adjacency_list* M, const unsigned i, class* c, elem* e) {
  list_unsigned_cell* p;
  unsigned k;
  p = M->r[i]->head;
  k = p->key;
  c[cls_ptr_h].h = c[cls_ptr_h].t = k;
  c[cls_ptr_h].s = 1;
  element_remove(e, k);
  element_insert(e, -1, k);
  e[k].b = 0;
  /* printf("e[%d].b = %d\n", k, cls_ptr_h); */
  /* element_print(e, elem_ptr_h); */
  for (p = p->next; NULL != p; p = p->next) {
    k = p->key;
    element_remove(e, k);
    element_insert(e, -1, k);
    e[k].b = 0;
    /* printf("element_ptr_h = %d\n", elem_ptr_h); */
    c[cls_ptr_h].s += 1;
    c[cls_ptr_h].t = k;
    /* printf("e[%d].b = %d\n", k, cls_ptr_h); */
    /* element_print(e, elem_ptr_h); */
  }
  MAX_CLASS_NUMBER = cls_ptr_h = cls_ptr_t = 0;
}

unsigned* get_c1p_order(matrix* M) {

  graph* G = make_overlap_graph(M);
  decomposing_to_connected_components(G);
  graph_print(G);

  class* c = init_class(M);
  elem* e = init_element(M);
  /* element_print(e, 0); */

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
    set_p0(Ma, p->key, c, e);
    /* element_print(e, p->key); */
    /* partition_print(c, e); */

    return NULL;
    
    for (p = p->next; NULL != p; p = p->next) {
      if (1 < Ma->r[p->key]->size) {
    	lr lr = refine(c, e, Ma->r[p->key]);
	return NULL;
    	/* if (!is_consecutive(lr)) { */
    	/*   /\* printf("Input Matrix has non-C1P\n"); *\/ */
  	/*   partitions_clear(P, num_c); */
  	/*   list_rows_clear(r, M->m); */
  	/*   columns_clear(Col); */
  	/*   graph_clear(G); */
  	/*   return NULL; */
    	/* } */
      }
      /* reset_do_intersect_P(P[k]); */
      /* reset_do_intersect_rows(r[p->key]); */
      /* reset_class_counter(P[k]); */
    }
    list_unsigned_clear(L); L = NULL;
  }
  adjacency_list_clear(Ma); Ma = NULL;
  graph_clear(G); G = NULL;
  
  return order;
}

list_unsigned* intersection_piAndT(elem* e, const int i, const list_unsigned* T) {
  list_unsigned* L = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  list_unsigned_cell* ptr;
  for (ptr = T->head; NULL != ptr; ptr = ptr->next)
    if (i == e[ptr->key].b)
      list_unsigned_add_rear(L, ptr->key);
  
  return L;
}

list_unsigned* intersection_SandT(elem* e, const list_unsigned* T) {
  list_unsigned* L = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  list_unsigned_cell* ptr;
  for (ptr = T->head; NULL != ptr; ptr = ptr->next)
    if (-1 != e[ptr->key].b)
      list_unsigned_add_rear(L, ptr->key);
  
  return L;
}

unsigned check_cases(const list_unsigned* T, const list_unsigned* SandT) {
  if (0 == SandT->size)
    return 1;

  if (SandT->size == T->size)
    return 2;

  return 3;
}

lr refine_sub(class* c, elem* e, const list_unsigned* T, const list_unsigned* SandT, const unsigned case_number) {
  lr lr = { 0, 1 };
  switch (case_number) {
  case 1: // このケースは C1P 判定では起こりえない（はず）CONFIRMME
    insert_new_class(c, e, T, cls_ptr_t);
    break;
  case 2:
    refine_2(c, e, SandT);
    break;
  default:
    ;
  }
  return lr;
}

void insert_new_class(class* c, elem* e, const list_unsigned* T, const unsigned i) {
  ++MAX_CLASS_NUMBER;
  const int prev = i;
  const int next = c[i].n;
  c[MAX_CLASS_NUMBER].p = prev;
  c[MAX_CLASS_NUMBER].n = next;
  c[prev].n = MAX_CLASS_NUMBER;
  if (-1 != next)
    c[next].p = MAX_CLASS_NUMBER;
  
  list_unsigned_cell* ptr = T->head;
  unsigned j, k;
  k = ptr->key;
  c[MAX_CLASS_NUMBER].h = c[MAX_CLASS_NUMBER].t = k;
  c[MAX_CLASS_NUMBER].s = 1;
  for (ptr = ptr->next; NULL != ptr; ptr = ptr->next) {
    j = k;
    k = ptr->key;
    e[j].n = k;
    e[k].b = MAX_CLASS_NUMBER;
    c[MAX_CLASS_NUMBER].s += 1;
    c[MAX_CLASS_NUMBER].t = k;
  }
}

void refine_2(class* c, elem* e, const list_unsigned* T) {
  unsigned prev_intersection_flag = false;
  prev_intersection_flag = refine_2_a(c, e, T);
  unsigned i;
  for (i = 1; i <= MAX_CLASS_NUMBER; ++i) {
    ;
    /* list_unsigned* ; */
  }
}

bool refine_2_a(class* c, elem* e, const list_unsigned* T) {
  list_unsigned* p_1INTt = intersection_piAndT(e, 1, T);
  bool flag = false;
  if (0 < p_1INTt->size) {
    flag = true;
    insert_new_class(c, e, p_1INTt, 1);
  }
  list_unsigned_clear(p_1INTt);
  return flag;
}

lr refine(class* c, elem* e, const list_unsigned* T) {
  lr lr = { 0, 1 };
  
  list_unsigned* sINTt = intersection_SandT(e, T);
  list_unsigned_print(sINTt); putchar('\n');
  const unsigned cc = check_cases(T, sINTt);
  printf("case %d\n", cc);

  lr = refine_sub(c, e, T, sINTt, cc);
  
  return lr;
}  

void partition_print(const class* c, const elem* e) {
  int i = cls_ptr_h;
  while (-1 != i) {
    int j = c[i].h;
    printf("{ %d(%d)", j, e[j].b);
    j = e[j].n;
    /* while (-1 != j) { */
    while (i == e[j].b) {
      printf(", %d(%d)", j, e[j].b);
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
