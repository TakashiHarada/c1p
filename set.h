/* set.h */

#include <stdio.h>
#include <stdlib.h>
/* #include <string.h> */

#ifndef __HARADALAB_KUT_BOOL_H__
#include "bool.h"
#endif

#ifndef __HARADALAB_KUT_SET_H__
#define __HARADALAB_KUT_SET_H__

typedef enum { red, black } color;

/* unsigned */
struct UNSIGNED_NODE {
  color color;
  unsigned key;
  struct UNSIGNED_NODE* p;
  struct UNSIGNED_NODE* left;
  struct UNSIGNED_NODE* right;
};
typedef struct UNSIGNED_NODE unsigned_node;

struct SET_UNSIGNED {
  unsigned size;
  unsigned_node* root;
  unsigned_node* nil;
};
typedef struct SET_UNSIGNED set_unsigned;

void set_unsigned_init(set_unsigned*);
bool set_unsigned_member(set_unsigned*, unsigned);
void set_unsigned_equal_sub(set_unsigned*, bool*, unsigned_node*, unsigned_node*);
bool set_unsigned_equal(set_unsigned*, set_unsigned*);
unsigned_node* tree_search_unsigned(unsigned_node*, unsigned_node*, unsigned);
unsigned tree_minimum_value_unsigned(set_unsigned*);
unsigned_node* tree_minimum_unsigned(unsigned_node*, unsigned_node*);
unsigned_node* tree_maximum_unsigned(unsigned_node*, unsigned_node*);
void left_lotate_unsigned(set_unsigned*, unsigned_node*);
void right_lotate_unsigned(set_unsigned*, unsigned_node*);
void set_unsigned_insert(set_unsigned*, unsigned);
void set_unsigned_insert_sub(set_unsigned*, unsigned_node*);
void set_instert_fixup_unsigned(set_unsigned*, unsigned_node*);
void set_transplant_unsigned(set_unsigned*, unsigned_node*, unsigned_node*);
void set_unsigned_delete(set_unsigned*, unsigned);
void set_delete_unsigned_sub(set_unsigned*, unsigned_node*);
void set_delete_fixup_unsigned(set_unsigned*, unsigned_node*);
bool set_unsigned_is_empty(set_unsigned*);
void set_unsigned_union(set_unsigned*, set_unsigned*);
void set_unsigned_union_sub(set_unsigned*, unsigned_node*, unsigned_node*);
set_unsigned* set_unsigned_intersect(set_unsigned*, set_unsigned*);
void set_unsigned_intersect_sub(set_unsigned*, set_unsigned*, unsigned_node*, unsigned_node*);
void set_unsigned_diff_sub(set_unsigned*, unsigned_node*, unsigned_node*);
void set_unsigned_diff(set_unsigned*, set_unsigned*);
void set_unsigned_copy(set_unsigned*, set_unsigned*);
void set_unsigned_copy_sub(set_unsigned*, unsigned_node*, unsigned_node*);
void set_unsigned_debug_print(unsigned_node*, unsigned_node*, unsigned);
void set_unsigned_print(set_unsigned*);
void set_unsigned_print_sub(unsigned_node*, unsigned_node*, bool*);
void set_unsigned_clear(set_unsigned*);
void set_unsigned_clear_sub(unsigned_node*, unsigned_node*);

/********** unsigned **********/

/* if S equals to T return true, otherwise false */
void set_unsigned_equal_sub(set_unsigned* S, bool* flag, unsigned_node* nil, unsigned_node* p) {
  if (NULL == p || nil == p || !flag) { return; }
  set_unsigned_equal_sub(S, flag, nil, p->left);
  set_unsigned_equal_sub(S, flag, nil, p->right);
  if (!set_unsigned_member(S,p->key)) { *flag = false; return; }
}

bool set_unsigned_equal(set_unsigned* S, set_unsigned* T) {
  bool flag = true;
  set_unsigned* tmp;
  if (T->size < S->size) { tmp = S; S = T; T = tmp; }
  set_unsigned_equal_sub(S, &flag, T->nil, T->root);
  return flag;
}

void set_unsigned_init(set_unsigned* T) {
  T->size = 0;
  T->nil = (unsigned_node*)calloc(1, sizeof(unsigned_node));
  T->nil->color = black;
  T->root = T->nil;
}

bool set_unsigned_member(set_unsigned* T, unsigned x) {
  if (T->nil != tree_search_unsigned(T->nil,T->root,x)) { return true; }
  else { return false; }
}
  
unsigned_node* tree_search_unsigned(unsigned_node* nil, unsigned_node* x, unsigned k) {
  if (nil == x || k == x->key) { return x; }
  if (k < x->key) { return tree_search_unsigned(nil, x->left, k); }
  else { return tree_search_unsigned(nil, x->right, k); }
}

unsigned tree_minimum_value_unsigned(set_unsigned* T) { /* assume that T has at least one element */
  unsigned_node* p = T->root;
  while (T->nil != p->left) { p = p->left; }
  return p->key;
}

unsigned_node* tree_minimum_unsigned(unsigned_node* nil, unsigned_node* x) {
  while (nil != x->left) { x = x->left; }
  return x;
}

unsigned_node* tree_maximum_unsigned(unsigned_node* nil, unsigned_node* x) { /* ; */
  while (nil != x->right) { x = x->right; }
  return x;
}
  
/* this subroutine assumes that NULL != x->right and NULL == T->nil */
void left_lotate_unsigned(set_unsigned* T, unsigned_node* x) {
  if (T->nil == x->right) { return ; }
  unsigned_node* y = x->right;
  x->right = y->left;
  if (T->nil != y->left) { y->left->p = x; }
  y->p = x->p;
  if (T->nil == x->p) { T->root = y; }
  else if (x == x->p->left) { x->p->left = y; }
  else { x->p->right = y; }
  y->left = x;
  x->p = y;
}

/* this subroutine assumes that NULL != y->left and NULL == T->nil */
void right_lotate_unsigned(set_unsigned* T, unsigned_node* y) {
  if (T->nil == y->left) { return ; }
  unsigned_node* x = y->left;
  y->left = x->right;
  if (T->nil != x->right) { x->right->p = y; }
  x->p = y->p;
  if (T->nil == y->p) { T->root = x; }
  else if (y == y->p->right) { y->p->right = x; }
  else { y->p->left = x; }
  x->right = y;
  y->p = x;
}

void set_unsigned_insert(set_unsigned* T, unsigned v) {
  if (T->nil != tree_search_unsigned(T->nil,T->root,v)) { return; }
  unsigned_node* z = (unsigned_node*)calloc(1,sizeof(unsigned_node));
  ++(T->size);
  z->key = v;
  z->left = z->right = z->p = T->nil;
  set_unsigned_insert_sub(T, z);
}

void set_unsigned_insert_sub(set_unsigned* T, unsigned_node* z) {
  unsigned_node* y = T->nil;
  unsigned_node* x = T->root;
  while (T->nil != x) {
    y = x;
    if (z->key < x->key) { x = x->left; }
    else { x = x->right; }
    z->p = y;
  }
  if (y == T->nil) { T->root = z; } /* T is empty */
  else if (z->key < y->key) { y->left = z; }
  else { y->right = z; }
  z->left = T->nil;
  z->right = T->nil;
  z->color = red;
  set_instert_fixup_unsigned(T, z);
}

void set_instert_fixup_unsigned(set_unsigned* T, unsigned_node* z) {
  unsigned_node* y;
  
  /* while (z->p != T->nil && red == z->p->color) { */
  while (red == z->p->color) {
    if (z->p == z->p->p->left) {
      y = z->p->p->right;
      /* if (T->nil != y && red == y->color) { /\* case 1 *\/ */
      if (red == y->color) { /* case 1 */
	z->p->color = black;
	y->color = black;
	z->p->p->color = red;
	z = z->p->p;
      }
      else {
	if (z == z->p->right) { /* case 2 */
	  z = z->p;
	  left_lotate_unsigned(T,z);
	}
	z->p->color = black; /* case 3 */
	z->p->p->color = red;
	right_lotate_unsigned(T,z->p->p);
      }
    }
    else {
      y = z->p->p->left;
      /* if (T->nil != y && red == y->color) { */
      if (red == y->color) {
	z->p->color = black;
	y->color = black;
	z->p->p->color = red;
	z = z->p->p;
      }
      else {
	if (z == z->p->left) {
	  z = z->p;
	  right_lotate_unsigned(T,z);
	}
	z->p->color = black;
	z->p->p->color = red;
	left_lotate_unsigned(T,z->p->p);
      }
    }
  }
  T->root->color = black;
}

void set_transplant_unsigned(set_unsigned* T, unsigned_node* u, unsigned_node* v) {
  if (u->p == T->nil) { T->root = v; }
  else if (u == u->p->left) { u->p->left = v; }
  else { u->p->right = v; }
  v->p = u->p;
}

void set_unsigned_delete(set_unsigned* T, unsigned v) {
  /* unsigned_node* z = (unsigned_node*)calloc(1,sizeof(unsigned_node)); */
  unsigned_node* z = tree_search_unsigned(T->nil,T->root,v);
  if (T->nil == z) { return; }
  --(T->size);
  set_delete_unsigned_sub(T, z);
}

void set_delete_unsigned_sub(set_unsigned* T, unsigned_node* z) {
  unsigned_node *x, *y = z;
  color y_original_color = y->color;
  if (z->left == T->nil) {
    x = z->right;
    set_transplant_unsigned(T,z,z->right);
  }
  else if (z->right == T->nil) {
    x = z->left;
    set_transplant_unsigned(T,z,z->left);
  } else {
    y = tree_minimum_unsigned(T->nil, z->right);
    y_original_color = y->color;
    x = y->right;
    if (y->p == z) { x->p = y; }
    else {
      set_transplant_unsigned(T,y,y->right);
      y->right = z->right;
      y->right->p = y;
    }
    set_transplant_unsigned(T,z,y);
    y->left = z->left;
    y->left->p = y;
    y->color = z->color;
  }
  if (black == y_original_color) { set_delete_fixup_unsigned(T,x); }
}

void set_delete_fixup_unsigned(set_unsigned* T, unsigned_node* x) {
  unsigned_node* w;
  while (x != T->root && black == x->color) {
    if (x == x->p->left) {
      w = x->p->right;
      if (red == w->color) {
	w->color = black;
	x->p->color = red;
	left_lotate_unsigned(T,x->p);
	w = x->p->right;
      }
      if (black == w->left->color && black == w->right->color) {
	w->color = red;
	x = x->p;
      }
      else {
	if (black == w->right->color) {
	  w->left->color = black;;
	  w->color = red;
	  right_lotate_unsigned(T,w);
	  w = x->p->right;
	}
	w->color = x->p->color;
	x->p->color = black;
	w->right->color = black;
	left_lotate_unsigned(T,x->p);
	x = T->root;
      }
    }
    else {
      w = x->p->left;
      if (red == w->color) {
	w->color = black;
	x->p->color = red;
	right_lotate_unsigned(T,x->p);
	w = x->p->left;
      }
      if (black == w->right->color && black == w->left->color) {
	w->color = red;
	x = x->p;
      }
      else {
	if (black == w->left->color) {
	  w->right->color = black;;
	  w->color = red;
	  left_lotate_unsigned(T,w);
	  w = x->p->left;
	}
	w->color = x->p->color;
	x->p->color = black;
	w->left->color = black;
	right_lotate_unsigned(T,x->p);
	x = T->root;
      }
    }
  }
  x->color = black;
}

bool set_unsigned_is_empty(set_unsigned* S) { if (NULL == S || S->root == S->nil) { return true; } return false; }

/* S := S \ T */
void set_unsigned_diff(set_unsigned* S, set_unsigned* T) {
  if (set_unsigned_is_empty(S) || set_unsigned_is_empty(T)) { return ; }
  set_unsigned_diff_sub(S, T->root, T->nil);
}

void set_unsigned_diff_sub(set_unsigned* S, unsigned_node* p, unsigned_node* nil) {
  if (nil == p || NULL == p) { return; }
  set_unsigned_diff_sub(S, p->left, nil);
  set_unsigned_diff_sub(S, p->right, nil);
  set_unsigned_delete(S, p->key);
}

/* S := S ∪ T */
void set_unsigned_union(set_unsigned* S, set_unsigned* T) { if (NULL == T) { return; } set_unsigned_union_sub(S, T->root, T->nil); }

void set_unsigned_union_sub(set_unsigned* S, unsigned_node* p, unsigned_node* nil) {
  if (nil == p || NULL == p) { return; }
  set_unsigned_union_sub(S, p->left, nil);
  set_unsigned_union_sub(S, p->right, nil);
  set_unsigned_insert(S, p->key);
}

/* R = S ∩ T */
set_unsigned* set_unsigned_intersect(set_unsigned* S, set_unsigned* T) {
  set_unsigned* R = (set_unsigned*)calloc(1,sizeof(set_unsigned));
  set_unsigned_init(R);
  if (NULL == T) { return NULL; }
  set_unsigned_intersect_sub(S, R, T->root, T->nil);
  return R;
}

void set_unsigned_intersect_sub(set_unsigned* S, set_unsigned* R, unsigned_node* p, unsigned_node* nil) {
  if (nil == p || NULL == p) { return; }
  set_unsigned_intersect_sub(S, R, p->left, nil);
  set_unsigned_intersect_sub(S, R, p->right, nil);
  if (set_unsigned_member(S, p->key)) { set_unsigned_insert(R, p->key); }
}

/* S := T */
void set_unsigned_copy(set_unsigned* S, set_unsigned* T) {
  set_unsigned_init(S);
  set_unsigned_copy_sub(S, T->root, T->nil);
}

void set_unsigned_copy_sub(set_unsigned* S, unsigned_node* p, unsigned_node* nil) {
  if (nil == p || NULL == p) { return; }
  set_unsigned_copy_sub(S, p->left, nil);
  set_unsigned_copy_sub(S, p->right, nil);
  set_unsigned_insert(S, p->key);
}

void set_unsigned_debug_print(unsigned_node* p, unsigned_node* nil, unsigned i) { /* post order */
  if (nil == p || NULL == p) { return; }
  set_unsigned_debug_print(p->left, nil, i+1);
  set_unsigned_debug_print(p->right, nil, i+1);
  printf("depth = %d: key = %d -- %d\n", i, p->key, p->color);
}

void set_unsigned_print(set_unsigned* S) { /* post order */
  if (NULL == S || S->nil == S->root) { return; }
  bool flag = false;
  set_unsigned_print_sub(S->root, S->nil, &flag);
}

void set_unsigned_print_sub(unsigned_node* p, unsigned_node* nil, bool* flag) { /* post order */
  if (nil == p || NULL == p) { return; }
  set_unsigned_print_sub(p->left, nil, flag);
  set_unsigned_print_sub(p->right, nil, flag);
  if (!(*flag)) { printf("%d", p->key); *flag = true; }
  else printf(", %d", p->key);
}

void set_unsigned_clear(set_unsigned* T) {
  if (NULL == T) { return; }
  unsigned_node* p = T->root;
  set_unsigned_clear_sub(T->nil, p);
  free(T->nil);
}

void set_unsigned_clear_sub(unsigned_node* nil, unsigned_node* p) {
  if (nil == p) { return; }
  set_unsigned_clear_sub(nil, p->left);
  set_unsigned_clear_sub(nil, p->right);
  free(p);
}

#endif
