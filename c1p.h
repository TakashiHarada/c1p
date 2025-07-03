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
  int h; // head of the part
  int t; // tail of the part
  int s; // size of the part
  int n; // next part
  int p; // previous part
  int counter; // for refine procedure
};
typedef struct CLASS part;

unsigned MAX_CLASS_NUMBER;
unsigned cls_ptr_h;
unsigned cls_ptr_t;

// insert 'i' next to the e[ptr]
// It is possible to i become to -1.
void part_insert(part* c, const int ptr, const int i) {
  /* printf("ptr = %d, i = %d, cls_ptr_h = %d\n", ptr, i, cls_ptr_h); */
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
    if (-1 != c[i].n) c[c[i].n].p = i;
    if (-1 == c[i].n) cls_ptr_t = i;
  }
}


struct ELEMENT {
  int b; // which part this element belongs to
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
    if (-1 != e[i].n) e[e[i].n].p = i;
  }
}

void element_remove(elem* e, const int k) {
  if (-1 != e[k].p) e[e[k].p].n = e[k].n;
  if (-1 != e[k].n) e[e[k].n].p = e[k].p;
  if (-1 == e[k].p) elem_ptr_h = e[k].n;
  
  e[k].b = -1;
  e[k].n = -1;
  e[k].p = -1;
}

void element_print(const elem* e, const int i) {
  printf("%d", i);
  int j = e[i].n;
  for ( ; -1 != j; j = e[j].n) printf(" --> %d", j);
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

bool* REFINED_CLASS_FLAG;
unsigned* REFINED_LAST_ELEMENT;

unsigned* get_c1p_order(matrix*);
part* init_part(matrix*);
elem* init_element(matrix*);
void set_p0(const adjacency_list*, const unsigned, part*, elem*);
void refine(part*, elem*, const list_unsigned*, const unsigned);
list_unsigned* intersection_SandT(const elem*, const list_unsigned*);
pair_list_unsigned* split(elem*, const list_unsigned*);
unsigned* set_order(const part*, const elem*, const unsigned);
unsigned check_cases(const list_unsigned*, const list_unsigned*);
unsigned check_cases_in_3(const elem*, const list_unsigned*);
bool check_cases_in_3b(const elem*, const list_unsigned*);
void refine_sub(part*, elem*, const pair_list_unsigned*, const unsigned, const unsigned);
void insert_new_part(part*, elem*, const list_unsigned*, const int);
void refine_2(part*, elem*, const list_unsigned*);
void refine_3_a(part*, elem*, const pair_list_unsigned*, const unsigned);
void refine_3_b(part*, elem*, const pair_list_unsigned*);
void refine_part(part*, elem*, const unsigned, const int);
/* void update_part(part*, elem*, const unsigned, const unsigned*); */
bool update_part(part*, elem*, const unsigned);
void update_part_3(part*, elem*, const unsigned, const unsigned*);
void clear_counter(part*);
void partition_print(const part*, const elem*);

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

part* init_part(matrix* M) {
  part* c = (part*)calloc(M->n, sizeof(part));

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

void set_p0(const adjacency_list* M, const unsigned i, part* c, elem* e) {
  list_unsigned_cell* p;
  unsigned k;
  p = M->r[i]->head;
  k = p->key;
  c[cls_ptr_h].h = c[cls_ptr_h].t = k;
  c[cls_ptr_h].s = 1;
  c[cls_ptr_h].p = c[cls_ptr_h].n = -1;
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

int CASES;

bool is_consecutive(part*, const elem* e, const list_unsigned*);

bool is_consecutive(part* c, const elem* e, const list_unsigned* T) { // FIXME
  /* printf("CASES = %d\n", CASES); */
  if (1 == CASES) return true;

  list_unsigned_cell* p;
  for (p = T->head; NULL != p; p = p->next) c[e[p->key].b].counter += 1;
  
  list_unsigned_cell* ptr = T->head;
  
  int i;
  unsigned sum = 0;
  for (i = e[ptr->key].b; -1 != i && 0 != c[i].counter; i = c[i].p) {
    sum += c[i].counter;
  }

  i = e[ptr->key].b;
  for (i = c[i].n; -1 != i && 0 != c[i].counter; i = c[i].n) {
    sum += c[i].counter;
  }

  for (ptr = T->head; NULL != ptr; ptr = ptr->next) {
    c[e[ptr->key].b].counter = 0;
  }
  
  if (T->size == sum) return true;

  return false;
}

unsigned* get_c1p_order(matrix* M) {

  graph* G = make_overlap_graph(M);
  decomposing_to_connected_components(G);
  /* graph_print(G); */

  part* c = init_part(M);
  elem* e = init_element(M);
  /* element_print(e, 0); */

  adjacency_list* Ma = matrix2adjacency_list(M);
  /* adjacency_list_print(Ma); */
					     
  unsigned num_c = G->num_of_components;
  printf("#component = %d\n", num_c);

  REFINED_CLASS_FLAG = (bool*)calloc(M->n, sizeof(bool));
  REFINED_LAST_ELEMENT = (unsigned*)calloc(M->n, sizeof(unsigned));
  
  unsigned k;
  bool init_flag = true;
  for (k = 0; k < num_c; ++k) {
    unsigned t = G->com[k]->size;
    list_unsigned* L = spanning_tree(G, k);

    /* if (1 == t) continue; */    
    /* printf("ST[%d] : ", k); list_unsigned_print(L); printf("\n"); */
    
    list_unsigned_cell* p = L->head;
    if (init_flag) {
      set_p0(Ma, p->key, c, e);
      p = p->next;
      init_flag = false;
    }
    /* element_print(e, elem_ptr_h); */
    /* partition_print(c, e); */
    
    for ( ; NULL != p; p = p->next) {
      if (1 < Ma->r[p->key]->size) {
    	refine(c, e, Ma->r[p->key], Ma->n);
    	if (!is_consecutive(c, e, Ma->r[p->key])) {
    	  printf("Input Matrix is non-C1P\n");
	  free(REFINED_CLASS_FLAG);
	  free(REFINED_LAST_ELEMENT);
	  adjacency_list_clear(Ma); Ma = NULL;
  	  graph_clear(G);
  	  return NULL;
    	}
      }
    }
    list_unsigned_clear(L); L = NULL;
  }
  
  free(REFINED_CLASS_FLAG);
  free(REFINED_LAST_ELEMENT);
  adjacency_list_clear(Ma); Ma = NULL;
  graph_clear(G); G = NULL;
  
  unsigned* order = set_order(c, e, M->n);  
  return order;
}

unsigned* set_order(const part* c, const elem* e, const unsigned n) {
  /* printf("cls_ptr_h = %u\n", cls_ptr_h); */
  unsigned* order = (unsigned*)calloc(n, sizeof(unsigned));
  int i, k;
  for (k = 0, i = cls_ptr_h; -1 != i; i = c[i].n) {
    int j = c[i].h;
    order[k] = j;
    for ( ; -1 != j && i == e[j].b; j = e[j].n) {
      order[k] = j;
      ++k;
    }
  }

  /* いずれのpartにも属さない列を右端にくっつける */
  for (i = elem_ptr_h; -1 != i; i = e[i].n)
    if (-1 == e[i].b) {
      // printf("%d\n", i);
      order[k] = i;
      ++k;
    }

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

bool check_cases_in_3b(const elem* e, const list_unsigned* SandT) {
  list_unsigned_cell* ptr;
  for (ptr = SandT->head; NULL != ptr; ptr = ptr->next)
    if (e[ptr->key].b == (int)cls_ptr_h)
      return true;
  return false;
}


void refine_sub(part* c, elem* e, const pair_list_unsigned* ST, const unsigned case_number, const unsigned n) {
  unsigned cc3;
  switch (case_number) {
  case 1:
    insert_new_part(c, e, ST->second, cls_ptr_t);
    break;
  case 2:
    /* list_unsigned_print(ST->first); putchar('\n'); */
    refine_2(c, e, ST->first);
    break;
  default:
    cc3 = check_cases_in_3(e, ST->first);
    /* printf("case [%d]\n", cc3); */
    /* list_unsigned_print(ST->first); putchar('\n'); */
    /* list_unsigned_print(ST->second); putchar('\n'); */
    switch (cc3) {
    case 1:
      refine_3_a(c, e, ST, n);
      break;
    default:
      refine_3_b(c, e, ST);
      break;
    }
  }
}

void insert_new_part(part* c, elem* e, const list_unsigned* T, const int i) {
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
    if (-1 == c[i].n)
      cls_ptr_t = MAX_CLASS_NUMBER;
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
    c[MAX_CLASS_NUMBER].h = k;
    prev = e[k].p;
  }
}

void refine_part(part* c, elem* e, const unsigned k, const int cls_num) {
  if (1 == c[cls_num].s)
    return;
  if (k == (unsigned)c[cls_num].h)
    return;
  if (k == (unsigned)c[cls_num].t)
    c[cls_num].t = e[k].p;
  element_remove(e, k);

  
    
  int prev = e[c[cls_num].h].p;
  /* printf("cls_num = %d, prev = %d\n", cls_num, prev); */
  element_insert(e, prev, k);
  /* printf("@@@ "); element_print(e, elem_ptr_h); */
  
  /* if (cls_num == (int)cls_ptr_h) */
  /*   elem_ptr_h = k; */
  c[cls_num].h = k;
  e[k].b = cls_num;
}

void clear_counter(part* c) {
  c->counter = 0;
}

bool update_part(part* c, elem* e, const unsigned cls_num) {
/* void update_part(part* c, elem* e, const unsigned cls_num, const unsigned* le) { */
  if (c[cls_num].s == c[cls_num].counter)
    return false;
  ++MAX_CLASS_NUMBER;

  /* printf("c[%u].s = %u, c[%u].count = %u\n", cls_num, c[cls_num].s, cls_num, c[cls_num].counter); */
  
  int i = c[cls_num].h;
  c[cls_num].s -= 1;
  e[i].b = MAX_CLASS_NUMBER;
  c[MAX_CLASS_NUMBER].h = c[MAX_CLASS_NUMBER].t = i;
  c[MAX_CLASS_NUMBER].s = 1;
  c[MAX_CLASS_NUMBER].counter = 1;
  if ((int)REFINED_LAST_ELEMENT[cls_num] != i) {
    for ( ; (int)REFINED_LAST_ELEMENT[cls_num] != i; i = e[i].n) {
      c[cls_num].s -= 1;
      e[i].b = MAX_CLASS_NUMBER;
      c[MAX_CLASS_NUMBER].t = i;
      c[MAX_CLASS_NUMBER].s += 1;
      c[MAX_CLASS_NUMBER].counter += 1;
    }
    e[i].b = MAX_CLASS_NUMBER;
    c[MAX_CLASS_NUMBER].t = i;
  }
  i = e[i].n;
  c[cls_num].h = i;

  /* printf("c[%d].p = %d\n", cls_num, c[cls_num].p); */
  
  if (-1 == c[cls_num].p)
    /* part_insert(c, -1, MAX_CLASS_NUMBER); */
    part_insert(c, cls_ptr_h, MAX_CLASS_NUMBER);
  else if (-1 != c[cls_num].p && 0 < c[c[cls_num].p].counter)
    part_insert(c, c[cls_num].p, MAX_CLASS_NUMBER);
  else
    part_insert(c, cls_num, MAX_CLASS_NUMBER);
  return true;
}

void refine_2(part* c, elem* e, const list_unsigned* T) {
  list_unsigned* refined_part = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  /* bool* refined_part_flag = (bool*)calloc(n, sizeof(bool)); */
  /* unsigned* refined_last_element = (unsigned*)calloc(n, sizeof(unsigned)); */

  /* element_print(e, elem_ptr_h); */
  list_unsigned_cell* ptr;
  for (ptr = T->head; NULL != ptr; ptr = ptr->next) {
    int cls_num = e[ptr->key].b;
    c[cls_num].counter += 1;
    /* if (!refined_part_flag[cls_num]) { */
    if (!REFINED_CLASS_FLAG[cls_num]) {
      list_unsigned_add_rear(refined_part, cls_num);
      /* refined_last_element[cls_num] = ptr->key; */
      REFINED_LAST_ELEMENT[cls_num] = ptr->key;
      /* refined_part_flag[cls_num] = true; */
      REFINED_CLASS_FLAG[cls_num] = true;
    }
    /* printf("### "); element_print(e, elem_ptr_h); */
    refine_part(c, e, ptr->key, cls_num);
    /* element_print(e, elem_ptr_h); */
  }


  
  list_unsigned* new_part = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  
  for (ptr = refined_part->head; NULL != ptr; ptr = ptr->next) {
    /* update_part(c, e, ptr->key, refined_last_element); */
    bool flag = update_part(c, e, ptr->key);
    if (flag)
      list_unsigned_add_rear(new_part, MAX_CLASS_NUMBER);
  }

  for (ptr = refined_part->head; NULL != ptr; ptr = ptr->next) {
    /* printf("[[%d]]\n", ptr->key); */
    REFINED_CLASS_FLAG[ptr->key] = false;
    clear_counter(&(c[ptr->key]));
  }

  for (ptr = new_part->head; NULL != ptr; ptr = ptr->next) {
    /* printf("[[%d]]\n", ptr->key); */
    REFINED_CLASS_FLAG[ptr->key] = false;
    clear_counter(&(c[ptr->key]));
  }

  
  /* free(refined_part_flag); refined_part_flag = NULL; */
  /* free(refined_last_element); refined_last_element = NULL; */
  list_unsigned_clear(refined_part); refined_part = NULL;
  list_unsigned_clear(new_part); new_part = NULL;
}

/* le means last element */
void update_part_3(part* c, elem* e, const unsigned cls_num, const unsigned* le) {
  if (c[cls_num].s == c[cls_num].counter)
    return;
  ++MAX_CLASS_NUMBER;

  int i = c[cls_num].h;
  c[cls_num].s -= 1;
  e[i].b = MAX_CLASS_NUMBER;
  c[MAX_CLASS_NUMBER].h = c[MAX_CLASS_NUMBER].t = i;
  c[MAX_CLASS_NUMBER].s = 1;
  if ((int)le[cls_num] != i) {    
    for ( ; (int)le[cls_num] != i; i = e[i].n) {
      c[cls_num].s -= 1;
      e[i].b = MAX_CLASS_NUMBER;
      c[MAX_CLASS_NUMBER].t = i;
      c[MAX_CLASS_NUMBER].s += 1;
    }
    /* c[cls_num].s -= 1; */
    e[i].b = MAX_CLASS_NUMBER;
    c[MAX_CLASS_NUMBER].t = i;
  }
  i = e[i].n;
  c[cls_num].h = i;
  /* printf("@ i = %d\n", i); */
  part_insert(c, -1, MAX_CLASS_NUMBER);
}

void refine_3_a(part* c, elem* e, const pair_list_unsigned* ST, const unsigned n) {
  list_unsigned* refined_part = (list_unsigned*)calloc(1, sizeof(list_unsigned));
  bool* refined_part_flag = (bool*)calloc(n, sizeof(bool));
  unsigned* refined_last_element = (unsigned*)calloc(n, sizeof(unsigned));

  /* putchar('@'); element_print(e, elem_ptr_h); */
  list_unsigned_cell* ptr;
  for (ptr = ST->first->head; NULL != ptr; ptr = ptr->next) {
    int cls_num = e[ptr->key].b;
    c[cls_num].counter += 1;
    if (!refined_part_flag[cls_num]) {
      list_unsigned_add_rear(refined_part, cls_num);
      refined_last_element[cls_num] = ptr->key;
      refined_part_flag[cls_num] = true;
    }
    refine_part(c, e, ptr->key, cls_num);
    /* printf("@@"); element_print(e, elem_ptr_h); */
  }

  for (ptr = refined_part->head; NULL != ptr; ptr = ptr->next)
    update_part_3(c, e, ptr->key, refined_last_element);

  /* printf("@@ "); partition_print(c, e); */
  for (ptr = refined_part->head; NULL != ptr; ptr = ptr->next)
    clear_counter(&(c[ptr->key]));

  /* list_unsigned_print(ST->second); putchar('\n'); */
  insert_new_part(c, e, ST->second, -1);
  
  free(refined_part_flag); refined_part_flag = NULL;
  free(refined_last_element); refined_last_element = NULL;
  list_unsigned_clear(refined_part); refined_part = NULL;
}

void refine_3_b(part* c, elem* e, const pair_list_unsigned* ST) {
  refine_2(c, e, ST->first);
  const bool cc3b = check_cases_in_3b(e, ST->first);
  if (cc3b) 
    insert_new_part(c, e, ST->second, -1);
  else
    insert_new_part(c, e, ST->second, cls_ptr_t);
}

void refine(part* c, elem* e, const list_unsigned* T, const unsigned n) {
  pair_list_unsigned* ST = split(e, T);
  const unsigned cc = check_cases(T, ST->first);
  CASES = cc;
  
  /* printf("case %d\n", cc); */
  /* partition_print(c,e); putchar('\n'); */

  refine_sub(c, e, ST, cc, n);
  
  /* putchar('#'), element_print(e, elem_ptr_h); */
  /* partition_print(c, e); putchar('\n'); */
  
  pair_list_unsigned_clear(ST);
}  

void partition_print(const part* c, const elem* e) {
  int i = cls_ptr_h;
  while (-1 != i) {
    int j = c[i].h;
    printf("{ %d(%d)", j, e[j].b);
    j = e[j].n;
    /* while (-1 != j) { */
    while (-1 != j && i == e[j].b) {
      printf(", %d(%d)", j, e[j].b);
      j = e[j].n;
    }
    /* printf(" }\n"); */
    /* printf("} [s = %u][c = %u]\n", c[i].s, c[i].counter); */
    printf("} [p = %d][n = %d][c = %u]\n", c[i].p, c[i].n, c[i].counter);
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
