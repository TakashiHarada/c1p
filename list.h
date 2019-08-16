/* list.h */

#ifndef __HARADALAB_KUT_LIST_H__
#define __HARADALAB_KUT_LIST_H__

#ifndef __HARADALAB_KUT_BOOL_H__
#include "bool.h"
#endif

#ifndef __HARADALAB_KUT_PAIR_H__
#include "pair.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/************************* list unsigned *************************/
struct LIST_UNSIGNED_CELL {
  unsigned key;
  struct LIST_UNSIGNED_CELL* prev;
  struct LIST_UNSIGNED_CELL* next;
};
typedef struct LIST_UNSIGNED_CELL list_unsigned_cell;

struct LIST_UNSIGNED {
  list_unsigned_cell* head;
  list_unsigned_cell* last;
  unsigned size;
};
typedef struct LIST_UNSIGNED list_unsigned;

bool list_unsigned_member(list_unsigned*, unsigned);
void list_unsigned_remove_head(list_unsigned*);
bool list_unsigned_is_empty(list_unsigned*);
list_unsigned_cell* list_unsigned_search(list_unsigned*, unsigned);
void list_unsigned_add_front(list_unsigned*, unsigned);
void list_unsigned_add_front_sub(list_unsigned*, list_unsigned_cell*);
void list_unsigned_add_rear(list_unsigned*, unsigned);
void list_unsigned_add_rear_sub(list_unsigned*, list_unsigned_cell*);
void list_unsigned_delete(list_unsigned*, unsigned);
void list_unsigned_delete_sub(list_unsigned*, list_unsigned_cell*);
list_unsigned* list_unsigneds_concat(list_unsigned*, list_unsigned*);
void list_unsigned_clear(list_unsigned*);
void list_unsigned_print(list_unsigned*);

bool list_unsigned_member(list_unsigned* L, unsigned u) {
  list_unsigned_cell* x;
  for (x = L->head; NULL != x; x = x->next)
    if (x->key == u) { return true; }
  return false;
}

void list_unsigned_remove_head(list_unsigned* L) { list_unsigned_delete_sub(L, L->head); }
bool list_unsigned_is_empty(list_unsigned* L) { return (L->size == 0); }

/* return the first cell containing u if there exists, otherwise return NULL */
list_unsigned_cell* list_unsigned_search(list_unsigned* L, unsigned u) {
  list_unsigned_cell* x = L->head;
  while (NULL != x && x->key != u)
    x = x->next;
  return x;
}

void list_unsigned_add_front(list_unsigned* L, unsigned u) {
  list_unsigned_cell* new = (list_unsigned_cell*)malloc(sizeof(list_unsigned_cell));
  L->size = L->size + 1;
  new->key = u;
  list_unsigned_add_front_sub(L,new);
}

void list_unsigned_add_front_sub(list_unsigned* L, list_unsigned_cell* x) {
  x->next = L->head;
  if (NULL != L->head) { L->head->prev = x; }
  else { L->last = x; }
  L->head = x;
  x->prev = NULL;
}

void list_unsigned_add_rear(list_unsigned* L, unsigned u) {
  list_unsigned_cell* new = (list_unsigned_cell*)malloc(sizeof(list_unsigned_cell));
  L->size = L->size + 1;
  new->key = u;
  list_unsigned_add_rear_sub(L,new);
}

void list_unsigned_add_rear_sub(list_unsigned* L, list_unsigned_cell* x) {
  x->prev = L->last;
  if (NULL != L->last) { L->last->next = x; } /* L is not empty */
  else { L->head = x; } /* L is empty */
  L->last = x;
  x->next = NULL;
}

void list_unsigned_delete(list_unsigned* L, unsigned u) {
  list_unsigned_cell* x = list_unsigned_search(L, u);
  if (NULL != x) { list_unsigned_delete_sub(L, x); }
}

/* remove the cell x from the list L */
void list_unsigned_delete_sub(list_unsigned* L, list_unsigned_cell* x) {
  L->size = L->size-1;
  if (NULL != x->prev) { x->prev->next = x->next; } 
  else { L->head = x->next; } // x is the head of L
  if (NULL != x->next) { x->next->prev = x->prev; }
  else { L->last = x->prev; } // x is the last of L
  free(x);
}

/* L1 := L1 ++ L2 */
list_unsigned* list_unsigneds_concat(list_unsigned* L1, list_unsigned* L2) {
  if (NULL == L1) { return L2; }
  if (NULL == L2) { return L1; }
  L1->last->next = L2->head;
  L2->head->prev = L1->last;
  L1->size = L1->size + L2->size;
  return L1;
}

void list_unsigned_clear(list_unsigned* L) {
  if (NULL == L)
    return;
  list_unsigned_cell *p, *q;
  for (p = L->head; NULL != p; ) {
    q = p;
    p = p->next;
    free(q);
  }
  free(L);
}

void list_unsigned_print(list_unsigned* L) {
  list_unsigned_cell* p = L->head;
  if (NULL != p) {
    printf("%u", p->key);
    for (p = p->next; NULL != p; p = p->next)
      printf(", %u", p->key);
  }
}


/* /\************************* list pair unsigned *************************\/ */
/* struct LIST_PAIR_UNSIGNED_CELL { */
/*   /\* unsigned first; *\/ */
/*   /\* unsigned second; *\/ */
/*   pair_unsigned pu; */
/*   struct LIST_PAIR_UNSIGNED_CELL* prev; */
/*   struct LIST_PAIR_UNSIGNED_CELL* next; */
/* }; */
/* typedef struct LIST_PAIR_UNSIGNED_CELL list_pair_unsigned_cell; */

/* struct LIST_PAIR_UNSIGNED { */
/*   list_pair_unsigned_cell* head; */
/*   list_pair_unsigned_cell* last; */
/*   unsigned size; */
/* }; */
/* typedef struct LIST_PAIR_UNSIGNED list_pair_unsigned; */

/* void list_pair_unsigned_remove_head(list_pair_unsigned*); */
/* pair_unsigned list_pair_unsigned_head(list_pair_unsigned*); */
/* bool list_pair_unsigned_is_empty(list_pair_unsigned* L); */
/* list_pair_unsigned_cell* list_pair_unsigned_search(list_pair_unsigned*, unsigned, unsigned); */
/* void list_pair_unsigned_insert2(list_pair_unsigned*, pair_unsigned); */
/* void list_pair_unsigned_insert(list_pair_unsigned*, unsigned, unsigned); */
/* void list_pair_unsigned_insert_sub(list_pair_unsigned*, list_pair_unsigned_cell*); */
/* void list_pair_unsigned_delete(list_pair_unsigned*, unsigned, unsigned); */
/* void list_pair_unsigned_delete_sub(list_pair_unsigned*, list_pair_unsigned_cell*); */
/* list_pair_unsigned* list_pair_unsigneds_concat(list_pair_unsigned*, list_pair_unsigned*); */
/* void list_pair_unsigned_clear(list_pair_unsigned*); */
/* void list_pair_unsigned_print(list_pair_unsigned*); */

/* void list_pair_unsigned_remove_head(list_pair_unsigned* L) { */
/*   list_pair_unsigned_delete_sub(L, L->head); */
/* } */

/* pair_unsigned list_pair_unsigned_head(list_pair_unsigned* L) { */
/*   list_pair_unsigned_cell* h = L->head; */
/*   pair_unsigned p = { h->pu.first, h->pu.second }; */

/*   return p; */
/* } */

/* bool list_pair_unsigned_is_empty(list_pair_unsigned* L) { return (L->size == 0); } */

/* list_pair_unsigned_cell* list_pair_unsigned_search(list_pair_unsigned* L, unsigned f, unsigned s) { */
/*   list_pair_unsigned_cell* x = L->head; */
/*   while (NULL != x && (x->pu.first != f || x->pu.second != s)) { x = x->next; } */
/*   return x; */
/* } */

/* void list_pair_unsigned_insert2(list_pair_unsigned* L, pair_unsigned p) { */
/*   list_pair_unsigned_cell* new = (list_pair_unsigned_cell*)malloc(sizeof(list_pair_unsigned_cell)); */
/*   L->size = L->size + 1; */
/*   new->pu.first = p.first; */
/*   new->pu.second = p.second; */
/*   list_pair_unsigned_insert_sub(L,new); */
/* } */

/* void list_pair_unsigned_insert(list_pair_unsigned* L, unsigned f, unsigned s) { */
/*   list_pair_unsigned_cell* new = (list_pair_unsigned_cell*)malloc(sizeof(list_pair_unsigned_cell)); */
/*   L->size = L->size + 1; */
/*   new->pu.first = f; */
/*   new->pu.second = s; */
/*   list_pair_unsigned_insert_sub(L,new); */
/* } */

/* void list_pair_unsigned_insert_sub(list_pair_unsigned* L, list_pair_unsigned_cell* x) { */
/*   x->next = L->head; */
/*   if (NULL != L->head) { L->head->prev = x; } */
/*   else { L->last = x; } */
/*   L->head = x; */
/*   x->prev = NULL; */
/* } */

/* void list_pair_unsigned_delete(list_pair_unsigned* L, unsigned f, unsigned s) { */
/*   list_pair_unsigned_cell* x = list_pair_unsigned_search(L, f, s); */
/*   if (NULL != x) { list_pair_unsigned_delete_sub(L, x); } */
/* } */

/* void list_pair_unsigned_delete_sub(list_pair_unsigned* L, list_pair_unsigned_cell* x) { */
/*   L->size = L->size - 1; */
/*   if (NULL != x->prev) { x->prev->next = x->next; } */
/*   else { L->head = x->next; } */
/*   if (NULL != x->next) { x->next->prev = x->prev; } */
/*   else { L->last = x->prev; } */
/* } */

/* list_pair_unsigned* list_pair_unsigneds_concat(list_pair_unsigned* L1, list_pair_unsigned* L2) { */
/*   if (NULL == L1) { return L2; } */
/*   L1->last->next = L2->head; */
/*   if (NULL != L2) { L2->head->prev = L1->last; } */
/*   L1->size = L1->size + L2->size; */
/*   return L1; */
/* } */

/* void list_pair_unsigned_clear(list_pair_unsigned* L) { */
/*   list_pair_unsigned_cell* p, *q; */
/*   for (p = L->head; NULL != p; ) { */
/*     q = p; */
/*     p = p->next; */
/*     free(q); */
/*   } */
/* } */

/* void list_pair_unsigned_print(list_pair_unsigned* L) { */
/*   list_pair_unsigned_cell* p = L->head; */
/*   if (NULL != p) { */
/*     printf("(%u, %u)", p->pu.first, p->pu.second); */
/*     for (p = p->next; NULL != p; p = p->next) { printf(", (%u, %u)", p->pu.first, p->pu.second); } */
/*   } */
/* } */


/* /\************************* list string *************************\/ */
/* struct LIST_STRING_CELL { */
/*   char* key; */
/*   struct LIST_STRING_CELL* prev; */
/*   struct LIST_STRING_CELL* next; */
/* }; */
/* typedef struct LIST_STRING_CELL list_string_cell; */

/* struct LIST_STRING { */
/*   list_string_cell* head; */
/*   list_string_cell* last; */
/*   unsigned size; */
/* }; */
/* typedef struct LIST_STRING list_string; */

/* void list_string_remove_head(list_string*); */
/* char* list_string_head(list_string*); */
/* bool list_string_is_empty(list_string*); */
/* list_string_cell* list_string_search(list_string*, char*); */
/* void list_string_insert(list_string*, char*); */
/* void list_string_insert_sub(list_string*, list_string_cell*); */
/* void list_string_delete(list_string*, char*); */
/* void list_string_delete_sub(list_string*, list_string_cell*); */
/* void list_strings_concat(list_string*, list_string*); */
/* void list_string_clear(list_string*); */
/* void list_string_print(list_string*); */



/* void list_string_remove_head(list_string* L) { list_string_delete_sub(L, L->head); } */

/* char* list_string_head(list_string* L) { */
/*   list_string_cell* h = L->head; */
/*   unsigned l = strlen(h->key); */
/*   char* p = (char*)calloc(l+1, sizeof(char)); */
/*   strcpy(p, h->key); */

/*   return p; */
/* } */

/* bool list_string_is_empty(list_string* L) { return (L->size == 0); } */

/* list_string_cell* list_string_search(list_string* L, char* s) { */
/*   list_string_cell* x = L->head; */
/*   while (NULL != x && strcmp(x->key, s)) { x = x->next; } */
/*   return x; */
/* } */

/* void list_string_insert(list_string* L, char* s) { */
/*   list_string_cell* new = (list_string_cell*)malloc(sizeof(list_string_cell)); */
/*   L->size = L->size + 1; */
/*   unsigned l = strlen(s); */
/*   new->key = (char*)calloc(l+1, sizeof(char)); */
/*   strcpy(new->key, s); */
/*   list_string_insert_sub(L,new); */
/* } */

/* void list_string_insert_sub(list_string* L, list_string_cell* x) { */
/*   x->next = L->head; */
/*   if (NULL != L->head) { L->head->prev = x; } */
/*   else { L->last = x; } */
/*   L->head = x; */
/*   x->prev = NULL; */
/* } */

/* void list_string_delete(list_string* L, char* s) { */
/*   list_string_cell* x = list_string_search(L, s); */
/*   if (NULL != x) { list_string_delete_sub(L, x); } */
/* } */

/* void list_string_delete_sub(list_string* L, list_string_cell* x) { */
/*   L->size = L->size - 1; */
/*   if (NULL != x->prev) { x->prev->next = x->next; } */
/*   else { L->head = x->next; } */
/*   if (NULL != x->next) { x->next->prev = x->prev; } */
/*   else { L->last = x->prev; } */
/*   free(x->key); */
/*   free(x); */
/* } */

/* void list_strings_concat(list_string* L1, list_string* L2) { */
/*   if (NULL == L1 || NULL == L1->head) { L1 = L2; return ; } */
/*   L1->last->next = L2->head; */
/*   if (NULL != L2 && NULL != L2->head) { L2->head->prev = L1->last; } */
/*   L1->size = L1->size + L2->size; */
/* } */

/* void list_string_clear(list_string* L) { */
/*   if (NULL == L) { return ; } */
/*   list_string_cell* p, *q; */
/*   for (p = L->head; NULL != p; ) { */
/*     q = p; */
/*     p = p->next; */
/*     free(q->key); */
/*     free(q); */
/*   } */
/* } */

/* void list_string_print(list_string* L) { */
/*   list_string_cell* p = L->head; */
/*   if (NULL != p) { */
/*     printf("%s", p->key); */
/*     for (p = p->next; NULL != p; p = p->next) { printf(", %s", p->key); } */
/*   } */
/* } */

#endif
