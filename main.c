#include <stdio.h>

#include "crypt.h"
#include "matrix.h"
#include "io.h"

int main(int argc, char *argv[]) {

  Matrix *m1 = input_matrix();
  char* filename = "m1.matrix";
  print_matrix(m1);
  printf("Saivng matrix to file %s\n", filename);
  save_matrix(m1, filename);

  // multiply_by_scalar(m1, 2.0);
  // puts("After scalar multiplication by 2:");
  // print_matrix(m1);

  // divide_by_scalar(m1, 2.0);
  // puts("After scalar division by 2 :");
  // print_matrix(m1);

//   Matrix *m3 = transpose_matrix(m1);
//   puts("Transpose of matrix:");
//   print_matrix(m3);

//   Matrix *m4 = multiply_by_matrix(m1, m3);
//   puts("Multiplication of m1 and transpose:");
//   print_matrix(m4);

  // double det = find_the_determinant(m1);
  // printf("Determinant of m1: %lf\n", det);

//   Matrix *m5 = inverse_matrix(m1);
//   if (m5) {
//     puts("Inverse of m1:");
//     print_matrix(m5);
//   } else 
//     puts("Matrix m1 is non-invertible.");
  

//   Matrix *m6 = init_matrix("Matrix B", data, 2, 2);
//   int status = gaussian_elimination(m6);
//   printf("After Gaussian elimination (status: %d):\n", status);
//   print_matrix(m6);

free_matrix(m1);
//   free_matrix(m3);
//   free_matrix(m4);
//   free_matrix(m5);
//   free_matrix(m6);

  return 0;
}
