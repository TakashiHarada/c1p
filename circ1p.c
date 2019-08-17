#include "circ1p.h"

int main(const int argc, char** argv) {
  if (2 != argc) {
    fprintf(stderr, "Usage: $ ./main [matrix]\nExample: $ ./main Matrix.txt\n");
    exit(1);
  }
  matrix* M = read_matrix(argv[1]);
  matrix_print(M);
  putchar('\n');
  
  unsigned* ord = get_circ1p_order(M);

  if (NULL != ord) {
    printf("\nCirc1P\n");
    order_print(ord, M->n);
    putchar('\n');
    matrix_print_with_order(M, ord);
  }
  else
    printf("Non-Circ1P\n");
  
  free(ord); ord = NULL;
    matrix_clear(M);
  return 0;
}
