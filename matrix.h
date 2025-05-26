#ifndef _H_MATRIX_H_
#define _H_MATRIX_H_

typedef unsigned int uint;

typedef struct{
    char*   alias;
    double* matrix;
    uint    number_of_rows;
    uint    number_of_cols;
} Matrix;

Matrix* init_matrix(char*, double*, uint, uint);
void    free_matrix(Matrix*);

void    multiply_by_scalar(Matrix*, double);
void divide_by_scalar(Matrix*, double);
Matrix* multiply_by_matrix(const Matrix*, const Matrix*);
Matrix* inverse_matrix(const Matrix*);
Matrix* transpose_matrix(const Matrix*);

int     gaussian_elimination(Matrix*);
double  find_the_determinant(const Matrix*);

#endif