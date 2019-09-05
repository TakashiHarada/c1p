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
  int counter; // for refine procedure
};
typedef struct CLASS class;

unsigned MAX_CLASS_NUMBER;
unsigned cls_ptr_h;
unsigned cls_ptr_t;

// insert 'i' next to the e[ptr]
// It is possible to i become to -1.
void class_insert(class* c, const int ptr, const int i) {
  if (-1 == ptr) {
    c[i].n = cls_ptr_h;
    c[i].p = -1;
    c[cls_ptr_h].p = i;
    cls_ptr_h = i;
  }
  else {
    c[i].n = c[ptr].n;
    c[i].p = ptr;
    c[ptr].n = i;
    if (-1 != c[i].n)
      c[c[i].n].p = i;
  }
}


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

struct PAIR_LIST_UNSGINED {
  list_unsigned* first;
  list_unsigned* second;
};
typedef struct PAIR_LIST_UNSGINED pair_list_unsigned;

void pair_list_unsigned_clear(pair_list_unsigned*);

void pair_list_unsigned_clear(pair_list_unsigned* P) {
  list_unsigned_clear(P->first); P->first = NULL;
  list_unsigned_clear(P->second); P->second = NULL;
}

unsigned* get_c1p_order(matrix*);
class* init_class(matrix*);
elem* init_element(matrix*);
void set_p0(const adjacency_list*, const unsigned, class*, elem*);
void refine(class*, elem*, const list_unsigned*, const unsigned);
list_unsigned* intersection_SandT(const elem*, const list_unsigned*);
pair_list_unsigned* split(elem*, const list_unsigned*);
unsigned check_cases(const list_unsigned*, const list_unsigned*);
unsigned check_cases_in_3(const elem*, const list_unsigned*);
void refine_sub(class*, elem*, const list_unsigned*, const pair_list_unsigned*, const unsigned, const unsigned);
void insert_new_class(class*, elem*, const list_unsigned*, const int);
void refine_2(class*, elem*, const list_unsigned*, const unsigned);
void refine_3_a(class*, elem*, const pair_list_unsigned*, const unsigned);
void refine_class(class*, elem*, const unsigned, const int);
void update_class(class*, elem*, const unsigned, const unsigned*);
void update_class_3(class*, elem*, const unsigned, const unsigned*);
void clear_counter(class*);
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
    c[i].counter = 0;
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
    c[cls_ptr_h].h = k;
    /* printf("e[%d].b = %d\n", k, cls_ptr_h); */
    /* element_print(e, elem_ptr_h); */
  }
  MAX_CLASS_NUMBER = cls_ptr_h = cls_ptr_t = 0;
}

unsigned most_left;
unsigned most_right;

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
    
    for (p = p->next; NULL != p; p = p->next) {
      if (1 < Ma->r[p->key]->size) {
    	refine(c, e, Ma->r[p->key], Ma->n);
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
    }
    list_unsigned_clear(L); L = NULL;
  }
  adjacency_list_clear(Ma); Ma = NULL;
  graph_clear(G); G = NULL;
  
  return order;
}

pair_list_unsigned* split(elem* e, const list_unsigned* T) {
  pair_list_unsigned* p = (pair_list_unsigned*)calloc(1, sizeof(pair_list_unsigned));
  list_unsigned* F = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  list_unsigned* S = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  list_unsigned_cell* ptr;
  for (ptr = T->head; NULL != ptr; ptr = ptr->next)
    if (-1 != e[ptr->key].b)
      list_unsigned_add_rear(F, ptr->key);
    else
      list_unsigned_add_rear(S, ptr->key);
  p->first = F, p->second = S;
  return p;
}

list_unsigned* intersection_SandT(const elem* e, const list_unsigned* T) {
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

unsigned check_cases_in_3(const elem* e, const list_unsigned* SandT) {
  list_unsigned_cell* ptr;
  for (ptr = SandT->head; NULL != ptr; ptr = ptr->next)
    if (e[ptr->key].b != (int)cls_ptr_h)
      return 2;
  return 1;
}

void refine_sub(class* c, elem* e, const list_unsigned* T, const pair_list_unsigned* ST, const unsigned case_number, const unsigned n) {
  unsigned cc3;
  switch (case_number) {
  case 1: // このケースは C1P 判定では起こりえない（はず）CONFIRMME
    insert_new_class(c, e, T, cls_ptr_t);
    break;
  case 2:
    refine_2(c, e, ST->first, n);
    break;
  default:
    cc3 = check_cases_in_3(e, ST->first);
    printf("case [%d]\n", cc3);
    switch (cc3) {
    case 1:
      refine_3_a(c, e, ST, n);
      break;
    default:
      refine_2(c, e, ST->first, n);
      break;
    }
  }
}

void insert_new_class(class* c, elem* e, const list_unsigned* T, const int i) {
  ++MAX_CLASS_NUMBER;

  if (-1 == i) {
    c[MAX_CLASS_NUMBER].n = cls_ptr_h;
    c[MAX_CLASS_NUMBER].p = -1;
    c[cls_ptr_h].p = MAX_CLASS_NUMBER;
    cls_ptr_h = MAX_CLASS_NUMBER;
  }
  else {
    const int prev = i;
    const int next = c[i].n;
    c[MAX_CLASS_NUMBER].p = prev;
    c[MAX_CLASS_NUMBER].n = next;
    c[prev].n = MAX_CLASS_NUMBER;
    if (-1 != next)
      c[next].p = MAX_CLASS_NUMBER;
  }
  
  list_unsigned_cell* ptr = T->head;
  unsigned k = ptr->key;;
  c[MAX_CLASS_NUMBER].h = c[MAX_CLASS_NUMBER].t = k;
  c[MAX_CLASS_NUMBER].s = 1;
  int prev = -1;
  if (-1 != i)
    prev = e[k].p;
  element_remove(e, k);
  element_insert(e, prev, k);
  e[k].b = MAX_CLASS_NUMBER;
  prev = e[k].p;
  
  for (ptr = ptr->next; NULL != ptr; ptr = ptr->next) {
    k = ptr->key;
    element_remove(e, k);
    /* e[j].n = k; */
    element_insert(e, prev, k);
    e[k].b = MAX_CLASS_NUMBER;
    c[MAX_CLASS_NUMBER].s += 1;
    c[MAX_CLASS_NUMBER].t = k;
    prev = e[k].p;
  }
}

void refine_class(class* c, elem* e, const unsigned k, const int cls_num) {
  if (1 == c[cls_num].s)
    return;
  if (k == (unsigned)c[cls_num].h)
    return;
  if (k == (unsigned)c[cls_num].t)
    c[cls_num].t = e[k].p;
  element_remove(e, k);

  int prev = e[c[cls_num].h].p;
  element_insert(e, prev, k);
  if (cls_num == (int)cls_ptr_h)
    elem_ptr_h = k;
  c[cls_num].h = k;
  e[k].b = cls_num;
}

void clear_counter(class* c) {
  c->counter = 0;
}

void update_class(class* c, elem* e, const unsigned cls_num, const unsigned* le) {
  if (c[cls_num].s == c[cls_num].counter)
    return;
  ++MAX_CLASS_NUMBER;

  int i = c[cls_num].h;
  e[i].b = MAX_CLASS_NUMBER;
  c[MAX_CLASS_NUMBER].h = c[MAX_CLASS_NUMBER].t = i;
  if ((int)le[cls_num] != i) {
    for ( ; (int)le[cls_num] != i; i = e[i].n) {
      e[i].b = MAX_CLASS_NUMBER;
      c[MAX_CLASS_NUMBER].t = i;
    }
    e[i].b = MAX_CLASS_NUMBER;
    c[MAX_CLASS_NUMBER].t = i;
  }
  i = e[i].n;
  c[cls_num].h = i;

  if (-1 != c[cls_num].p && 0 < c[c[cls_num].p].counter)
    class_insert(c, -1, MAX_CLASS_NUMBER);
  else
    class_insert(c, cls_num, MAX_CLASS_NUMBER);
}

void refine_2(class* c, elem* e, const list_unsigned* T, const unsigned n) {
  list_unsigned* refined_class = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  bool* refined_class_flag = (bool*)calloc(n, sizeof(bool));
  unsigned* refined_last_element = (unsigned*)calloc(n, sizeof(unsigned));

  element_print(e, elem_ptr_h);
  list_unsigned_cell* ptr;
  for (ptr = T->head; NULL != ptr; ptr = ptr->next) {
    int cls_num = e[ptr->key].b;
    c[cls_num].counter += 1;
    if (!refined_class_flag[cls_num]) {
      list_unsigned_add_rear(refined_class, cls_num);
      refined_last_element[cls_num] = ptr->key;
      refined_class_flag[cls_num] = true;
    }
    refine_class(c, e, ptr->key, cls_num);
    element_print(e, elem_ptr_h);
  }

  for (ptr = refined_class->head; NULL != ptr; ptr = ptr->next)
    update_class(c, e, ptr->key, refined_last_element);

  /* partition_print(c, e); */
  for (ptr = refined_class->head; NULL != ptr; ptr = ptr->next)
    clear_counter(&(c[ptr->key]));
  
  free(refined_class_flag); refined_class_flag = NULL;
  free(refined_last_element); refined_last_element = NULL;
  list_unsigned_clear(refined_class); refined_class = NULL;
}

void update_class_3(class* c, elem* e, const unsigned cls_num, const unsigned* le) {
  if (c[cls_num].s == c[cls_num].counter)
    return;
  ++MAX_CLASS_NUMBER;

  int i = c[cls_num].h;
  e[i].b = MAX_CLASS_NUMBER;
  c[MAX_CLASS_NUMBER].h = c[MAX_CLASS_NUMBER].t = i;
  if ((int)le[cls_num] != i) {
    for ( ; (int)le[cls_num] != i; i = e[i].n) {
      e[i].b = MAX_CLASS_NUMBER;
      c[MAX_CLASS_NUMBER].t = i;
    }
    e[i].b = MAX_CLASS_NUMBER;
    c[MAX_CLASS_NUMBER].t = i;
  }
  i = e[i].n;
  c[cls_num].h = i;

  class_insert(c, -1, MAX_CLASS_NUMBER);
}

void refine_3_a(class* c, elem* e, const pair_list_unsigned* ST, const unsigned n) {
  list_unsigned* refined_class = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  bool* refined_class_flag = (bool*)calloc(n, sizeof(bool));
  unsigned* refined_last_element = (unsigned*)calloc(n, sizeof(unsigned));

  element_print(e, elem_ptr_h);
  list_unsigned_cell* ptr;
  for (ptr = ST->first->head; NULL != ptr; ptr = ptr->next) {
    int cls_num = e[ptr->key].b;
    c[cls_num].counter += 1;
    if (!refined_class_flag[cls_num]) {
      list_unsigned_add_rear(refined_class, cls_num);
      refined_last_element[cls_num] = ptr->key;
      refined_class_flag[cls_num] = true;
    }
    refine_class(c, e, ptr->key, cls_num);
    element_print(e, elem_ptr_h);
  }

  for (ptr = refined_class->head; NULL != ptr; ptr = ptr->next)
    update_class_3(c, e, ptr->key, refined_last_element);

  /* partition_print(c, e); */
  for (ptr = refined_class->head; NULL != ptr; ptr = ptr->next)
    clear_counter(&(c[ptr->key]));

  /* list_unsigned_print(ST->second); putchar('\n'); */
  insert_new_class(c, e, ST->second, -1);
  
  free(refined_class_flag); refined_class_flag = NULL;
  free(refined_last_element); refined_last_element = NULL;
  list_unsigned_clear(refined_class); refined_class = NULL;
}

void refine(class* c, elem* e, const list_unsigned* T, const unsigned n) {
  pair_list_unsigned* ST = split(e, T);
  const unsigned cc = check_cases(T, ST->first);
  
  printf("case %d\n", cc);
  /* partition_print(c,e); */
  
  refine_sub(c, e, T, ST, cc, n);
  partition_print(c, e);
  element_print(e, elem_ptr_h);
  pair_list_unsigned_clear(ST);
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
