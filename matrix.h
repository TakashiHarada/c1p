/* matrix.h */

#ifndef __HARADALAB_KUT_MATRIX_H__
#define __HARADALAB_KUT_MATRIX_H__

#include <stdlib.h>

struct MATRIX {
  unsigned m; // number of rows
  unsigned n; // number of columns
  char**   b;
};
typedef struct MATRIX matrix;

matrix* read_matrix(char*);
matrix* matrix_copy(const matrix*);
void matrix_print(const matrix*);
void matrix_print_with_order(const matrix*, const unsigned*);
void matrix_clear(matrix*);

matrix* read_matrix(char* filename) {
   FILE* fp;
  if (NULL == (fp = fopen(filename,"r"))) {
    fprintf(stderr,"ERROR: Can't read the graph file.\n");
    exit(1);
  }
  char* line = NULL;
  size_t len = 0;
  unsigned m = 0;
  unsigned n = 0;
  if (-1 == getline(&line, &len, fp)) { exit(1); }
  ++m, n = strlen(line)-1;
  while (-1 != getline(&line, &len, fp)) { ++m; }

  matrix* M = (matrix*)calloc(1, sizeof(matrix));
  M->b = (char**)calloc(m, sizeof(char*));
  M->m = m;
  M->n = n;
  
  unsigned i, j;
  rewind(fp);
  for (i = 0; getline(&line, &len, fp) != -1; ++i) {
    M->b[i] = (char*)calloc(n+1, sizeof(char));
    for (j = 0; j < n; ++j) { M->b[i][j] = line[j]; }
    M->b[i][n] = '\0';
  }
  fclose(fp);
  return M;
}

matrix* matrix_copy(const matrix* M) {
  matrix* N = (matrix*)calloc(1, sizeof(matrix));
  N->m = M->m;
  N->n = M->n;

  N->b = (char**)calloc(M->m, sizeof(char*));
  unsigned i;
  for (i = 0; i < M->m; ++i) {
    unsigned j;
    N->b[i] = (char*)calloc(M->n, sizeof(char));
    for (j = 0; j < M->n; ++j)
      N->b[i][j] = M->b[i][j];
  }
  
  return N;
}

void matrix_print(const matrix* M) {
  unsigned i, j;
  for (i = 0; i < M->m; ++i) {
    for (j = 0; j < M->n; ++j) putchar(M->b[i][j]);
    if (i != M->m-1)
      putchar('\n');
  }
}

void matrix_print_with_order(const matrix* M, const unsigned* s) {
  unsigned i;
  for (i = 0; i < M->m; ++i) {
    unsigned j;
    for (j = 0; j < M->n; ++j) putchar(M->b[i][s[j]]);
    putchar('\n');
  }
}

void matrix_clear(matrix* M) {
  unsigned i;
  for (i = 0; i < M->m; ++i) {
    free(M->b[i]);
    M->b[i] = NULL;
  }
  free(M->b);
  M->b = NULL;
  free(M);
}

#endif
