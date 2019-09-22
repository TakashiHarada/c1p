/* c1p.h */

#ifndef __HARADALAB_KUT_CIRC1P_H__
#define __HARADALAB_KUT_CIRC1P_H__

#ifndef __HARADALAB_KUT_C1P_H__
#include "c1p.h"
#endif

void complementing_row(matrix*, const unsigned);
void complementing_rows_with_1_in_the_first_column(matrix*);
unsigned* get_circ1p_order(const matrix*);
bool check_circ1p_matrix(const matrix*);

bool check_circ1p_matrix(const matrix* M) {
  matrix* N = matrix_copy(M);
  complementing_rows_with_1_in_the_first_column(N);
  /* matrix_print(M); */
  unsigned* ord = get_c1p_order(N);
  bool flag = false;
  if (NULL != ord)
    flag = true;
  free(ord); ord = NULL;
  matrix_clear(N); N = NULL;
  return flag;
}

unsigned* get_circ1p_order(const matrix* M) {
  matrix* N = matrix_copy(M);
  complementing_rows_with_1_in_the_first_column(N);
  /* matrix_print(M); */
  unsigned* ord = get_c1p_order(N);
  matrix_clear(N); N = NULL;
  return ord;
}

void complementing_rows_with_1_in_the_first_column(matrix* M) {
  unsigned i;
  for (i = 0; i < M->m; ++i)
    if ('1' == M->b[i][0])
      complementing_row(M, i);
}

void complementing_row(matrix* M, const unsigned i) {
  unsigned j;
  for (j = 0; j < M->n; ++j)
    if ('1' == M->b[i][j])
      M->b[i][j] = '0';
    else
      M->b[i][j] = '1';
}

#endif
