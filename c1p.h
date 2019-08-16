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

struct COLUMN_CELL {
  unsigned key;             /* column number */
  struct CLASS* cls;        /* pointer to a class */
  struct COLUMN_CELL* prev; /* pointer to the previous cell */
  struct COLUMN_CELL* next; /* pointer to the next cell */
  bool do_intersect_T;      /* for refine */
};
typedef struct COLUMN_CELL column_cell;

struct COLUMNS {
  column_cell* head;
  column_cell* last;
  unsigned size;
};
typedef struct COLUMNS columns;

struct LIST_ROW_CELL {
  column_cell* key;
  struct LIST_ROW_CELL *prev;
  struct LIST_ROW_CELL *next;
};
typedef struct LIST_ROW_CELL list_row_cell;

struct LIST_ROW {
  list_row_cell* head;
  list_row_cell* last;
  unsigned size;
};
typedef struct LIST_ROW list_row;

/* class is called "part" in other paper */
struct CLASS {
  unsigned cls_num;   // class number;
  unsigned size;
  column_cell* head;    // pointer to the first element of the class
  column_cell* last;    // pointer to the last element of the class
  struct CLASS* prev;
  struct CLASS* next;
  unsigned counter;   // for refine routine
  bool do_intersect;  // for refine routine
};
typedef struct CLASS class;

/* partition is a list of classes */
struct PARTITION {
  class* head;   // pointer to the first element of the partition
  class* last;   // pointer to the last element of the partition
  unsigned size; // the number of classes
};
typedef struct PARTITION partition;

struct LEFT_RIGHT { // p.33 Definition 2.2.8 
  struct CLASS* l; // smallest integer (class) such that P_l ∩ T ≠ ∅
  struct CLASS* r; // largest integer (class) such that P_r ∩ T ≠ ∅
};
typedef struct LEFT_RIGHT lr;

void order_print(const unsigned*, const unsigned);
bool check_strongC1P_matrix(matrix*);
unsigned* get_c1p_order(matrix*);

columns* init_columns(matrix*);
void columns_add_rear(columns*, unsigned);
void columns_add_rear_sub(columns*, column_cell*);
void columns_print(columns*);
void columns_clear(columns*);

list_row** make_list_rows(matrix*, columns*);
void list_row_add_rear(list_row*, column_cell*);
void list_row_add_rear_sub(list_row*, list_row_cell*);
void list_rows_print(list_row**, const unsigned);
void list_row_print(list_row*);
void list_rows_clear(list_row**, const unsigned);
void list_row_clear(list_row*);

partition* set_P1(list_row**, columns*, unsigned);
void columns_move(columns*, column_cell*, column_cell*);
lr refine(partition*, columns*, list_row*);
lr refine_case_1(partition*, columns*, list_row*);
lr refine_case_2(partition*, columns*, list_row*);
lr refine_case_3(partition*, columns*, list_row*);
void reset_class_counter(partition*);
void reset_do_intersect_P(partition*);
void reset_do_intersect_rows(list_row*);
void reset_cls_pointer(columns*);

bool is_consecutive(lr);
void partition_print(partition*);
void partitions_clear(partition**, const unsigned);
void partition_clear(partition*);

void order_print(const unsigned* s, const unsigned n) {
  unsigned i;
  for (i = 0; i < n; ++i)
    printf("%d", s[i]);
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

unsigned MAX_CLS_NUM;

unsigned* get_c1p_order(matrix* M) {
  graph* G = make_overlap_graph(M);
  decomposing_to_connected_components(G);
  /* graph_print(G); */
  
  unsigned* order = (unsigned*)calloc(M->n, sizeof(unsigned));
  columns* Col = init_columns(M);
  /* columns_print(Col); putchar('\n'); */

  list_row** r = make_list_rows(M, Col);
  /* list_rows_print(r, M->m); */

  unsigned num_c = G->num_of_components;
  /* printf("#component = %d\n", num_c); */
  
  partition** P = (partition**)calloc(num_c, sizeof(partition*));
  unsigned k;
  for (k = 0; k < num_c; ++k) {
    unsigned t = G->com[k]->size;
    if (1 == t)
      continue;
    list_unsigned* L = spanning_tree(G, k);
    /* printf("ST[%d] : ", k); list_unsigned_print(L); printf("\n"); */

    list_unsigned_cell* p = L->head;
    P[k] = set_P1(r, Col, p->key);
    
    MAX_CLS_NUM = 1;
    /* printf("\n"); partition_print(P[k]); printf("\n"); */
    for (p = p->next; NULL != p; p = p->next) {
      if (1 < r[p->key]->size) {
    	lr lr = refine(P[k], Col, r[p->key]);
    	if (!is_consecutive(lr)) {
    	  /* printf("Input Matrix has non-C1P\n"); */
	  partitions_clear(P, num_c);
	  list_rows_clear(r, M->m);
	  columns_clear(Col);
	  graph_clear(G);
	  return NULL;
    	}
      }
      reset_do_intersect_P(P[k]);
      reset_do_intersect_rows(r[p->key]);
      reset_class_counter(P[k]);
    }
    list_unsigned_clear(L); L = NULL;
    reset_cls_pointer(Col);
  }

  column_cell* q;
  for (k = 0, q = Col->head; NULL != q; q = q->next, ++k)
    order[k] = q->key;

  partitions_clear(P, num_c);
  list_rows_clear(r, M->m);
  columns_clear(Col);
  graph_clear(G);
  
  return order;
}

columns* init_columns(matrix* M) {
  columns* Col = (columns*)calloc(1, sizeof(columns));
  Col->head = Col->last = NULL;
  unsigned i;
  for (i = 0; i < M->n; ++i)
    columns_add_rear(Col, i);
  return Col;
}

void columns_add_rear(columns* Col, unsigned v) {
  column_cell* new = (column_cell*)calloc(1, sizeof(column_cell));
  Col->size = Col->size + 1;
  new->key = v;
  new->do_intersect_T = false;
  new->cls = NULL;
  columns_add_rear_sub(Col, new);
}

void columns_add_rear_sub(columns* Col, column_cell* x) {
  x->prev = Col->last;
  if (NULL != Col->last) { Col->last->next = x; }
  else { Col->head = x; }
  Col->last = x;
  x->next = NULL;
}

void columns_print(columns* Col) {
  column_cell* x = Col->head;
  if (NULL != x) {
    printf("Col : %d", x->key);
    for (x = x->next; NULL != x; x = x->next)
      printf(", %d", x->key);
  }
}

void columns_clear(columns* Col) {
  if (NULL == Col) { return; }
  column_cell *p, *q;
  for (p = Col->head; NULL != p; ) {
    q = p;
    p = p->next;
    free(q);
  }
  free(Col); Col = NULL;
}

/* each row r_i is a set of the column number j, s.t. M_ij = 1 */
list_row** make_list_rows(matrix* M, columns* Col) {
  list_row** r = (list_row**)calloc(M->m, sizeof(list_row*));
  unsigned i, j;
  column_cell** ptr = (column_cell**)calloc(M->n, sizeof(column_cell*));
  column_cell* p;
  for (j = 0, p = Col->head; NULL != p; ++j, p = p->next)
    ptr[j] = p;
  for (i = 0; i < M->m; ++i) {
    r[i] = (list_row*)calloc(1, sizeof(list_row));
    for (j = 0; j < M->n; ++j)
      if ('1' == M->b[i][j])
	list_row_add_rear(r[i], ptr[j]);
  }
  free(ptr);
  return r;
}

void list_row_add_rear(list_row* r, column_cell* v) {
  list_row_cell* new = (list_row_cell*)calloc(1, sizeof(list_row_cell));
  r->size = r->size+1;
  new->key = v;
  list_row_add_rear_sub(r, new);
}

void list_row_add_rear_sub(list_row* r, list_row_cell* x) {
  x->prev = r->last;
  if (NULL != r->last) { r->last->next = x; }
  else { r->head = x; }
  r->last = x;
  x->next = NULL;
}

void list_rows_print(list_row** r, const unsigned m) {
  unsigned i;
  for (i = 0; i < m; ++i) {
    printf("r[%d] : ", i);
    list_row_print(r[i]);
    putchar('\n');
  }
}

void list_row_print(list_row* r) {
  list_row_cell* p = r->head;
  if (NULL != p) {
    printf("%d", p->key->key);
    for (p = p->next; NULL != p; p = p->next)
      printf(", %d", p->key->key);
  }
}

void list_rows_clear(list_row** r, const unsigned m) {
  unsigned i;
  for (i = 0; i < m; ++i) {
    list_row_clear(r[i]);
    r[i] = NULL;
  }
  free(r);
}

void list_row_clear(list_row* r) {
  if (NULL == r) { return; }
  list_row_cell *p, *q;
  for (p = r->head; NULL != p; ) {
    q = p;
    p = p->next;
    free(q);
  }
  free(r);
  r = NULL;
}

partition* set_P1(list_row** r, columns* Col, unsigned v) {
  partition* P1 = (partition*)calloc(1, sizeof(partition));
  class* C = (class*)calloc(1, sizeof(class));
  C->cls_num = 0;
  C->counter = 0;
  C->prev = C->next = NULL;  // pointers for classes (parts)
  C->head = r[v]->head->key; // pointer for column cell
  C->last = NULL;            // pointer for column cell
  C->do_intersect = false;   // for refine routine
  P1->head = P1->last = C;
  P1->size = 1;
  
  list_row_cell* p;
  column_cell* q = r[v]->head->key;
  /* column_cell* q = Col->head; */
  for (p = r[v]->head; NULL != p; p = p->next) {
    /* printf("p points %d cell, q points %d cell\n", p->key->key, q->key); */
    if (p->key != q) {
      columns_move(Col, p->key, q);
      /* columns_print(Col); putchar('\n'); */
    }
    else
      q = q->next;
    p->key->cls = C;
    C->last = p->key;
  }
  C->size = r[v]->size;
  
  return P1;
}

/* move the cell pointed by x to the previous of the cell y */
void columns_move(columns* Col, column_cell* x, column_cell* y) {
  if (x == y) { return; }
  /* remove x from Col */
  if (NULL == x->prev) { Col->head = x->next; } /* x is the first element of Col */
  else { x->prev->next = x->next; }
  if (NULL == x->next) { /* x is the last element of E */
    Col->last = x->prev;
    Col->last->next = NULL;
  } 
  else { x->next->prev = x->prev; }
  /* insert x to the previous of y */
  if (NULL == y->prev) { /* y is the first element of E */
    Col->head = x;
    x->prev = NULL;
  }
  else { x->prev = y->prev; x->prev->next = x; }
  y->prev = x;
  x->next = y;
}

/* move the cell pointed by x to the next of the cell y */
void columns_insert_next(columns* Col, column_cell* x, column_cell* y) {
  if (x == y) { return; }
  /* remove x from E */
  if (NULL == x->prev) { Col->head = x->next; } /* x is the first element of Col */
  else { x->prev->next = x->next; }
  if (NULL == x->next) { Col->last = x->prev; } /* x is the last element of Col */
  else { x->next->prev = x->prev; }
  /* insert x to the next of y */
  if (NULL == y->next) { Col->last = x; } /* y is the last element of Col */
  else { x->next = y->next; x->next->prev = x; }
  y->next = x;
  x->prev = y;
}

lr refine(partition* P, columns* Col, list_row* T) {
  /* printf("refine by {"); vset_cell_list_print(T); printf("}\n"); */

  /* collect the class C_i to be refined i.e., 
     C_i is not a subset of T and intersection of C_i and T is not the empty */
  /* check whether case 1 or 2 (a), (b), (3) or 3 (a) (b) */
  list_row_cell* x;
  unsigned is_not_S = 0;
  unsigned is_S = 0;
  for (x = T->head; NULL != x; x = x->next) {
    if (NULL != x->key->cls) {
      x->key->do_intersect_T = true;
      ++x->key->cls->counter;
      ++is_S;
    }
    else { ++is_not_S; }
  }

  /* refine */
  if (0 == is_S) { // case 1.
    /* printf(" case 1\n"); */
    return refine_case_1(P, Col, T);
  }
  else if (0 == is_not_S) { // case 2.
    /* printf(" case 2 "); */
    return refine_case_2(P, Col, T);
  }
  // case 3.
  /* printf(" case 3 "); */
  /* partition_print(P); */
  return refine_case_3(P, Col, T);
}

lr refine_case_1(partition* P, columns* Col, list_row* T) {
  column_cell* s = P->last->last->next;
  class* new_class = (class*)calloc(1, sizeof(class));
  new_class->cls_num = MAX_CLS_NUM; ++MAX_CLS_NUM;
  new_class->counter = 0;
  new_class->prev = P->last;
  new_class->next = NULL;
  new_class->head = T->head->key;
  new_class->do_intersect = true;
  P->last->next = new_class;
  P->last = new_class;
  ++(P->size);

  list_row_cell* r;
  for (r = T->head; NULL != r; r = r->next) {
    if (r->key != s) { columns_move(Col, r->key, s); }
    else { s = s->next; }
    r->key->cls = new_class;
    new_class->last = s;
  }
  new_class->size = T->size;

  lr lr;
  lr.l = new_class;
  lr.r = new_class;

  return lr;
}

lr refine_case_2(partition* P, columns* E, list_row* T) {
  class* C;
  list_row_cell* r;

  lr lr = { NULL, NULL };
  for (C = P->head; NULL != C; ) {
    if (0 != C->counter && C->counter < C->size) { // to be refined
      if (P->head == C) { // (a)
	/* printf("(a)\n"); */
	/* P->head (p1) is used as p1_intersect_T */
	class* p1_minus_T = (class*)calloc(1, sizeof(class));
	p1_minus_T->cls_num = MAX_CLS_NUM; ++MAX_CLS_NUM;
	p1_minus_T->counter = 0;
	p1_minus_T->prev = NULL;
	p1_minus_T->next = C;
	p1_minus_T->do_intersect = false;
	P->head = p1_minus_T;
	C->prev = p1_minus_T;
	++(P->size);

	p1_minus_T->size = 0;
	p1_minus_T->head = p1_minus_T->last = NULL;
	column_cell* c;
	for (c = C->head; C->last->next != c; c = c->next) {
	  if (!c->do_intersect_T) {
	    if (NULL == p1_minus_T->head) { p1_minus_T->head = c; }
	    if (c == C->head) { C->head = C->head->next; }
	    if (c == C->last) { C->last = C->last->prev; }
	    columns_move(E, c, C->head);
	    c->cls = p1_minus_T;
	    --(C->size);
	    ++(p1_minus_T->size);
	    p1_minus_T->last = c;
	  }
	}
	lr.l = lr.r = C;
	C->do_intersect = true;
	C = C->next;
      }
      else if (!C->prev->do_intersect) { // (b)
	/* printf("(b)\n"); */
	class* p1_minus_T = (class*)calloc(1, sizeof(class));
	p1_minus_T->cls_num = MAX_CLS_NUM; ++MAX_CLS_NUM;
	p1_minus_T->counter = 0;
	p1_minus_T->prev = C->prev;
	p1_minus_T->next = C;
	C->prev->next = p1_minus_T;
	C->prev = p1_minus_T;
	p1_minus_T->do_intersect = false;
	++(P->size);

	p1_minus_T->size = 0;
	p1_minus_T->head = p1_minus_T->last = NULL;
	column_cell* c;
	for (c = C->head; C->last->next != c; c = c->next)
	  if (!c->do_intersect_T) {
	    if (NULL == p1_minus_T->head) { p1_minus_T->head = c; }
	    if (c == C->head) { C->head = C->head->next; }
	    if (c == C->last) { C->last = C->last->prev; }
	    columns_move(E, c, C->head);
	    c->cls = p1_minus_T;
	    --(C->size);
	    ++(p1_minus_T->size);
	    p1_minus_T->last = c;
	  }
	if (NULL == lr.l) { lr.l = C; }
	lr.r = C;
	C->do_intersect = true;
	C = C->next;
      }
      else { // (c)
	/* printf("(c)\n"); */
	class* p1_intersect_T = (class*)calloc(1, sizeof(class));
	p1_intersect_T->cls_num = MAX_CLS_NUM; ++MAX_CLS_NUM;
	p1_intersect_T->counter = 0;
	p1_intersect_T->prev = C->prev;
	p1_intersect_T->next = C;
	C->prev->next = p1_intersect_T;
	C->prev = p1_intersect_T;
	p1_intersect_T->do_intersect = true;
	++(P->size);
	if (NULL == lr.l) { lr.l = p1_intersect_T; }
	lr.r = p1_intersect_T;

	p1_intersect_T->size = 0;
	p1_intersect_T->head = p1_intersect_T->last = NULL;
	for (r = T->head; NULL != r; r = r->next)
	  if (r->key->cls == C) {
	    if (NULL == p1_intersect_T->head) { p1_intersect_T->head = r->key; }
	    if (r->key == C->head) { C->head = C->head->next; }
	    if (r->key == C->last) { C->last = C->last->prev; }
	    columns_move(E, r->key, C->head);
	    r->key->cls = p1_intersect_T;
	    --(C->size);
	    ++(p1_intersect_T->size);
	    p1_intersect_T->last = r->key;
	  }
	C->do_intersect = false;
	C = C->next;
      }
    }
    else {
      /* printf("cls_num = %d\n", C->cls_num); */
      if (0 != C->counter) {
	if (NULL == lr.l) { lr.l = C; }
	lr.r = C;
	C->do_intersect = true;
      }
      C = C->next;
    }
  }

  return lr;
}

/* bool is_consecutive(lr lr, vset* P) { */
bool is_consecutive(lr lr) {
  class* p;
  for (p = lr.l; p != lr.r->next; p = p->next) {
    if (!p->do_intersect)
      return false;
  }
  return true;
}

lr refine_case_3(partition* P, columns* Col, list_row* T) {
  /* check whether case (a) or (b) */
  bool flag = false; // false denotes (a), true denotes (b)
  list_row_cell* r;
  reset_class_counter(P);
  reset_do_intersect_P(P);
  reset_do_intersect_rows(T);

  lr lr = { NULL, NULL };
  
  for (r = T->head; NULL != r; r = r->next) {
    if (NULL != r->key->cls && P->head != r->key->cls) { flag = true; }
    if (NULL != r->key->cls) {
      r->key->do_intersect_T = true;
      ++r->key->cls->counter;
    }
  }

  if (!flag) { // (a)
    /* printf("(a)\n"); */
    class* p0 = P->head;
    class* T_minus_S = (class*)calloc(1, sizeof(class));
    T_minus_S->prev = NULL;
    T_minus_S->head = T_minus_S->last = NULL;
    T_minus_S->size = 0;
    T_minus_S->cls_num = MAX_CLS_NUM; ++MAX_CLS_NUM;
    P->size += 1;
    T_minus_S->do_intersect = true;
    lr.l = T_minus_S;
    
    if (p0->counter == p0->size) {
      /* T_intersect_p0 is unnecessary
       * p0 is used as T_intersect_p0 and p0_minus_T
       */
      T_minus_S->next = p0;
      p0->prev = T_minus_S;
      p0->do_intersect = true;
      lr.r = p0;
      for (r = T->head; NULL != r; r = r->next) {
	column_cell* ptr = NULL;
	if (NULL == r->key->cls) {
	  if (NULL == T_minus_S->head) { ptr = p0->head; }
	  else { ptr = T_minus_S->head; }
	  columns_move(Col, r->key, ptr);
	  r->key->cls = T_minus_S;
	  T_minus_S->size += 1;
	  if (NULL == T_minus_S->last) { T_minus_S->last = r->key; }
	  T_minus_S->head = r->key;
	}
      }
    }
    else {
      /* p0 is used as p0_minus_T */
      class* T_intersect_p0 = (class*)calloc(1, sizeof(class));
      T_intersect_p0->size = 0;
      T_intersect_p0->cls_num = MAX_CLS_NUM; ++MAX_CLS_NUM;
      T_minus_S->next = T_intersect_p0;
      T_intersect_p0->prev = T_minus_S;
      T_intersect_p0->next = p0;
      p0->prev = T_intersect_p0;
      T_intersect_p0->head = T_intersect_p0->last = NULL;
      P->size += 1;
      T_intersect_p0->do_intersect = true;
      lr.r = T_intersect_p0;
      
      for (r = T->head; NULL != r; r = r->next) {
	/* printf("r->key->key = %d\n", r->key->key); */
	column_cell* ptr = NULL;
	if (p0 == r->key->cls) {
	  if (NULL == T_intersect_p0->head) { ptr = p0->head; }
	  else { ptr = T_intersect_p0->head; }
	  if (r->key == p0->head) { p0->head = p0->head->next; }
	  if (r->key == p0->last) { p0->last = p0->last->prev; }
	  columns_move(Col, r->key, ptr);
	  
	  r->key->cls = T_intersect_p0;
	  T_intersect_p0->size += 1;
	  p0->size -= 1;
	  /* printf("T ∩ p0\n"); */
	  if (NULL == T_intersect_p0->last) { T_intersect_p0->last = r->key; }
	  T_intersect_p0->head = r->key;
	}
	if (NULL == r->key->cls) {
	  if (NULL == T_minus_S->head) {
	    if (NULL == T_intersect_p0->head) { ptr = p0->head; }
	    else { ptr = T_intersect_p0->head; }
	  }
	  else { ptr = T_minus_S->head; }
	  columns_move(Col, r->key, ptr);
	  r->key->cls = T_minus_S;
	  T_minus_S->size += 1;
	  /* printf("T \\ S\n"); */
	  if (NULL == T_minus_S->last) { T_minus_S->last = r->key; }
	  T_minus_S->head = r->key;
	}
      }
    }
    P->head = T_minus_S;
  }
  else { // (b)
    /* printf("(b)\n"); */
    list_row* TS = (list_row*)calloc(1, sizeof(list_row));
    for (r = T->head; NULL != r; r = r->next)
      if (NULL != r->key->cls)
	list_row_add_rear(TS, r->key);

    lr = refine_case_2(P, Col, TS);
    /* partition_print(P); */

    /* check refine part of head or last */
    bool flag = false;
    /* false denotes to refine last, true denotes to refine head */
    for (r = T->head; NULL != r; r = r->next)
      if (r->key->cls == P->head) {
	flag = true; break;
      }

    /* recompute the counters for each classes */
    reset_class_counter(P);
    for (r = T->head; NULL != r; r = r->next) {
      if (NULL != r->key->cls)
	++r->key->cls->counter;
    }
    
    if (flag) { // refine p_0
      /* printf("refine p0\n"); */
      class* T_minus_S = (class*)calloc(1, sizeof(class));
      T_minus_S->cls_num = MAX_CLS_NUM; ++MAX_CLS_NUM;
      T_minus_S->head = T_minus_S->last = NULL;
      T_minus_S->size = 0;
      T_minus_S->counter = 0;
      T_minus_S->do_intersect = true;
      P->size += 1;

      class* p0 = P->head;
      if (p0->counter == p0->size) {
	/* T_intersect_p0 is unnecessary */
	/* p0 is used as p0_intersect_T and p0_minus_T */
	T_minus_S->next = p0;
	p0->do_intersect = true;
	lr.l = T_minus_S;
	for (r = T->head; NULL != r; r = r->next) {
	  column_cell* ptr = NULL;
	  if (NULL == r->key->cls) {
	    if (NULL == T_minus_S->head) { ptr = p0->head; }
	    else { ptr = T_minus_S->head; }
	    columns_move(Col, r->key, ptr);
	    r->key->cls = T_minus_S;
	    T_minus_S->size += 1;
	    if (NULL == T_minus_S->last) { T_minus_S->last = r->key; }
	    T_minus_S->head = r->key;
	  }
	}
	p0->prev = T_minus_S;
      }
      else {
	/* p0 is used as p0_minus_T */
	class* p0_intersect_T = (class*)calloc(1, sizeof(class));
	p0_intersect_T->cls_num = MAX_CLS_NUM; ++MAX_CLS_NUM;
	p0_intersect_T->counter = 0;
	T_minus_S->next = p0_intersect_T;
	p0_intersect_T->prev = T_minus_S;
	p0_intersect_T->next = p0;
	p0_intersect_T->head = p0_intersect_T->last = NULL;
	p0_intersect_T->do_intersect = true;
	p0->prev = p0_intersect_T;
	P->size += 1;

	for (r = T->head; NULL != r; r = r->next) {
	  column_cell* ptr = NULL;
	  if (p0 == r->key->cls) {
	    if (NULL == p0_intersect_T->head) { ptr = p0->head; }
	    else { ptr = p0_intersect_T->head; }
	    if (r->key == p0->head) { p0->head = p0->head->next; }
	    if (r->key == p0->last) { p0->last = p0->last->prev; }
	    columns_move(Col, r->key, ptr);
	    if (ptr == p0->head) { p0->head = p0->head->next; }

	    r->key->cls = p0_intersect_T;
	    p0_intersect_T->size += 1;
	    p0->size -= 1;
	    if (NULL == p0_intersect_T->last) {
	      p0_intersect_T->last = r->key;
	      p0->head = p0->head->next;
	    }
	    p0_intersect_T->head = r->key;
	  }
	  if (NULL == r->key->cls) {
	    if (NULL == T_minus_S->head) {
	      if (NULL == p0_intersect_T->head) { ptr = p0->head; }
	      else { ptr = p0_intersect_T->head; }
	    }
	    else { ptr = T_minus_S->head; }
	    columns_move(Col, r->key, ptr);
	    r->key->cls = T_minus_S;
	    T_minus_S->size += 1;
	    if (NULL == T_minus_S->last) { T_minus_S->last = r->key; }
	    T_minus_S->head = r->key;
	  }
	}
      }
      P->head = T_minus_S;
    }
    else { // refine p_v
      class* T_minus_S = (class*)calloc(1, sizeof(class));
      T_minus_S->cls_num = MAX_CLS_NUM; ++MAX_CLS_NUM;
      T_minus_S->head = T_minus_S->last = NULL;
      T_minus_S->size = 0;
      T_minus_S->counter = 0;
      P->size += 1;
      T_minus_S->next = NULL;
      T_minus_S->do_intersect = true;
      lr.r = T_minus_S;
      class* pv = P->last;
      /* FIXMEX? */
      /* if (pv->counter == pv->size) { */ 
      if (0 == pv->counter || pv->counter == pv->size) {
	/* pv_intersect_T is unnecessary
	 * pv is used as pv_minus_T and pv_intersect_T
	 */
	/* printf("pv ∩ T is unnecessary\n"); */
	/* partition_print(P); */
	T_minus_S->prev = pv;
	pv->next = T_minus_S;
	for (r = T->head; NULL != r; r = r->next) {
	  column_cell* ptr = NULL;
	  if (NULL == r->key->cls) {
	    if (NULL == T_minus_S->head) { ptr = pv->last; }
	    else { ptr = T_minus_S->last; }
	    /* printf("elm = %d\n", ptr->key); */
	    columns_insert_next(Col, r->key, ptr);
	    r->key->cls = T_minus_S;
	    T_minus_S->size += 1;
	    if (NULL == T_minus_S->head) { T_minus_S->head = r->key; }
	    T_minus_S->last = r->key;
	  }
	}
      }
      else {
	/* pv is used as pv_minus_T */
	class* pv_intersect_T = (class*)calloc(1, sizeof(class));
	pv_intersect_T->cls_num = MAX_CLS_NUM; ++MAX_CLS_NUM;
	pv_intersect_T->counter = 0;
	T_minus_S->prev = pv_intersect_T;
	pv_intersect_T->prev = pv;
	pv_intersect_T->next = pv_intersect_T;
	pv_intersect_T->head = pv_intersect_T->last = NULL;
	pv->next = pv_intersect_T;
	pv_intersect_T->do_intersect = true;
	P->size += 1;

	for (r = T->head; NULL != r; r = r->next) {
	  column_cell* ptr = NULL;
	  if (pv == r->key->cls) {
	    if (NULL == pv_intersect_T->last) { ptr = pv->last; }
	    else { ptr = pv_intersect_T->last; }
	    columns_insert_next(Col, r->key, ptr);
	    if (ptr == pv->last) { pv->last = pv->last->prev; }

	    r->key->cls = pv_intersect_T;
	    pv_intersect_T->size += 1;
	    pv->size -= 1;
	    if (NULL == pv_intersect_T->head) {
	      pv_intersect_T->head = r->key;
	      pv->last = pv->last->prev;
	    }
	    pv_intersect_T->last = r->key;
	  }
	  if (NULL == r->key->cls) {
	    if (NULL == T_minus_S->last) {
	      if (NULL == pv_intersect_T->last) { ptr = pv->last; }
	      else { ptr = pv_intersect_T->last; }
	    }
	    else { ptr = T_minus_S->last; }
	    columns_insert_next(Col, r->key, ptr);
	    r->key->cls = T_minus_S;
	    T_minus_S->size += 1;
	    if (NULL == T_minus_S->head) { T_minus_S->head = r->key; }
	    T_minus_S->last = r->key;
	  }
	}
      }
      P->last = T_minus_S;
    }
  }
  return lr;
}

void reset_class_counter(partition* P) {
  class* p;
  for (p = P->head; NULL != p; p = p->next)
    p->counter = 0;
}

void reset_do_intersect_P(partition* P) {
  class* p;
  for (p = P->head; NULL != p; p = p->next)
    p->do_intersect = false;
}

void reset_do_intersect_rows(list_row* T) {
  list_row_cell* r;
  for (r = T->head; NULL != r; r = r->next)
    r->key->do_intersect_T = false;
}

void reset_cls_pointer(columns* Col) {
  column_cell* p;
  for (p = Col->head; NULL != p; p = p->next)
    p->cls = NULL;
}


void partition_print(partition* P) {
  if (NULL == P) { return; }
  class* C;
  column_cell* col;
  for (C = P->head; NULL != C; C = C->next) {
    printf("P[%d] : ", C->cls_num);
    col = C->head;
    if (NULL == col) { putchar('\n'); continue; }
    if (NULL != col) {
      if (C->size == 1) { printf("%d\n", col->key); } // |C| = 1
      else {      // |C| > 1
	printf("%d", col->key);
	for (col = col->next; C->last->next != col; col = col->next)
	  printf(", %d", col->key);
	putchar('\n');
      }
    }
  }
}

void partitions_clear(partition** P, const unsigned n) {
  if (NULL == P) { return ; }
  unsigned i;
  for (i = 0; i < n; ++i) {
    partition_clear(P[i]);
    P[i] = NULL;
  }
}

void partition_clear(partition* P) {
  if (NULL == P) { return; }
  class *C, *D;
  for (C = P->head; NULL != C; ) {
    D = C;
    C = C->next;
    free(D);
    D = NULL;
  }
  free(P);
}

#endif
