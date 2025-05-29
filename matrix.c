#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

/* TODO: Correct matrix passed check to all of the functions */

#define APROX_ZERO 1e-10

Matrix* init_matrix(char* arg_alias, double* arg_matrix_array, uint arg_number_of_rows, uint arg_number_of_cols){
    Matrix* temp_matrix = (Matrix*)malloc(sizeof(Matrix));
    if(temp_matrix == NULL){
        fprintf(stderr, "Trouble allocating memory for matrix (%s)!☆\n", __func__);
        exit(1);
    }

    temp_matrix->alias = malloc(strlen(arg_alias) + 1);
    if (temp_matrix->alias == NULL){
        fprintf(stderr, "Trouble allocating memory for alias!☆\n");
        exit(1);
    }
    memcpy(temp_matrix->alias, arg_alias, strlen(arg_alias) + 1);

    if(arg_number_of_rows <= 0 || arg_number_of_cols <= 0){
        fprintf(stderr, "Invalid argument (%s)!☆\n", __func__);
        exit(1);
    }
    temp_matrix->number_of_rows = arg_number_of_rows;
    temp_matrix->number_of_cols = arg_number_of_cols;

    temp_matrix->matrix = (double*)malloc(sizeof(double)*temp_matrix->number_of_cols*temp_matrix->number_of_rows);
    if(temp_matrix->matrix == NULL){
        fprintf(stderr, "Trouble allocating memory for matrix array (%s)!☆\n", __func__);
        exit(1);
    }
    for(int i = 0; i < temp_matrix->number_of_rows; i++){
        for(int j = 0; j < temp_matrix->number_of_cols; j++){
            temp_matrix->matrix[i*temp_matrix->number_of_cols + j] = 
            arg_matrix_array[i*temp_matrix->number_of_cols + j];
        }
    }

    return temp_matrix;
}

void multiply_by_scalar(Matrix* arg_matrix, double arg_scalar){

    for(int i = 0; i < arg_matrix->number_of_rows; i++){
        for(int j = 0; j < arg_matrix->number_of_cols; j++){
            arg_matrix->matrix[i*arg_matrix->number_of_cols + j] *= arg_scalar;
        }
    }

}

Matrix* multiply_by_matrix(const Matrix* arg_matrix_1, const Matrix* arg_matrix_2){
    if(arg_matrix_1->number_of_cols != arg_matrix_2->number_of_rows){
        fprintf(stderr,"Cannot multiply matrices (%s)!☆\n", __func__);
        exit(1);
    }

    Matrix* new_matrix = (Matrix*)malloc(sizeof(Matrix));
    if(new_matrix == NULL){
        fprintf(stderr,"Trouble allocating memory for matrix (%s)!☆\n", __func__);
        exit(1);
    }

    new_matrix->number_of_rows =  arg_matrix_1->number_of_rows;
    new_matrix->number_of_cols = arg_matrix_2->number_of_cols;

    new_matrix->matrix = (double*)malloc(sizeof(double)*new_matrix->number_of_rows*new_matrix->number_of_cols);
    if(new_matrix->matrix == NULL){
        fprintf(stderr,"Trouble allocating memory for matrix array (%s)!☆\n", __func__);
        exit(1);
    }

    for(int i = 0; i < arg_matrix_1->number_of_rows; i++){
        for(int j = 0; j < arg_matrix_2->number_of_cols; j++){
            new_matrix->matrix[i*new_matrix->number_of_cols + j] = 0;
            for(int k = 0; k < arg_matrix_1->number_of_cols; k++){
                new_matrix->matrix[i*new_matrix->number_of_cols + j] += 
                arg_matrix_1->matrix[i*arg_matrix_1->number_of_cols + k] * arg_matrix_2->matrix[k*arg_matrix_2->number_of_cols + j];
            }
        }
    }

    size_t len1 = strlen(arg_matrix_1->alias);
    size_t len2 = strlen(arg_matrix_2->alias);
    new_matrix->alias = (char*)malloc(len1 + len2 + 2);
    if(new_matrix->alias == NULL){
        fprintf(stderr, "Trouble allocating memory for matrix alias (%s)!☆\n", __func__);
        exit(1);
    }
    memcpy(new_matrix->alias, arg_matrix_1->alias, len1);
    new_matrix->alias[len1] = 'x';
    memcpy(&new_matrix->alias[len1 + 1], arg_matrix_2->alias, len2 + 1);

    return new_matrix;
}

void divide_by_scalar(Matrix* arg_matrix, double arg_scalar){
    if(arg_scalar == 0){
        fprintf(stderr, "Attempted division by 0 (%s)\n", __func__);
    }
    for(int i = 0; i < arg_matrix->number_of_rows; i++){
        for(int j = 0; j < arg_matrix->number_of_cols; j++){
            arg_matrix->matrix[i*arg_matrix->number_of_cols + j] /= arg_scalar;
        }
    }
}

int gaussian_elimination(Matrix* arg_matrix){
    int number_of_swaps = 0;
    for(int i = 0; i < arg_matrix->number_of_rows; i++){
        for(int k = i + 1; k < arg_matrix->number_of_rows; k++){
            if(fabs(arg_matrix->matrix[i*arg_matrix->number_of_cols + i]) 
            < fabs(arg_matrix->matrix[k*arg_matrix->number_of_cols + i])){
                ++number_of_swaps;
                for(int j = 0; j < arg_matrix->number_of_cols; j++){
                    double temp;
                    temp = arg_matrix->matrix[i*arg_matrix->number_of_cols + j];
                    arg_matrix->matrix[i*arg_matrix->number_of_cols + j] =  
                    arg_matrix->matrix[k*arg_matrix->number_of_cols + j];

                    arg_matrix->matrix[k*arg_matrix->number_of_cols + j] = temp;
                }
            }
        }

        double pivot = arg_matrix->matrix[i*arg_matrix->number_of_cols + i];
        if(fabs(pivot) < APROX_ZERO) return -1;

        for(int k = i + 1; k < arg_matrix->number_of_rows; k++){
            double term = arg_matrix->matrix[k*arg_matrix->number_of_cols + i] / pivot;
            for(int j = 0; j < arg_matrix->number_of_cols; j++){
                arg_matrix->matrix[k*arg_matrix->number_of_cols + j] -= term*arg_matrix->matrix[i*arg_matrix->number_of_cols + j];
            }
        }
    }

    return number_of_swaps;
}

double find_the_determinant(const Matrix* arg_matrix){
    if(arg_matrix->number_of_cols != arg_matrix->number_of_rows){
        fprintf(stderr, "Passing a matrix that isn't square (%s)!☆\n", __func__);
        exit(1);
    }

    Matrix* temp_matrix = init_matrix(arg_matrix->alias, arg_matrix->matrix, arg_matrix->number_of_rows, arg_matrix->number_of_cols);

    double determinant = 1;

    int number_of_swaps = gaussian_elimination(temp_matrix);
    if(number_of_swaps == -1){
        return 0;
    }

    for(int i = 0; i < temp_matrix->number_of_cols; i++){
        determinant *= temp_matrix->matrix[i*temp_matrix->number_of_cols + i];
    }
    determinant *= pow(-1, number_of_swaps);
    free_matrix(temp_matrix);
    return determinant;
}

double minor_matrix_determinant(const Matrix* arg_matrix, int arg_row, int arg_col){
    if(arg_matrix->number_of_rows != arg_matrix->number_of_cols){
        fprintf(stderr, "Passing a matrix that isn't square (%s)!☆\n", __func__);
        exit(1);
    }
    Matrix* return_matrix = NULL;
    int new_rows = arg_matrix->number_of_rows - 1, new_cols = arg_matrix->number_of_cols - 1;
    double* buffer = (double*)malloc(sizeof(double)*new_rows*new_cols);
    char* new_alias = (char*)malloc(sizeof(char)*(strlen(arg_matrix->alias) + strlen("_minor") + 1));

    memcpy(new_alias, arg_matrix->alias, strlen(arg_matrix->alias));
    memcpy(&new_alias[strlen(arg_matrix->alias)], "_minor\0", strlen("_minor") + 1);

    int minor_i = 0;
    for(int i = 0; i < arg_matrix->number_of_rows; i++){
        if(i == arg_row){
            continue;
        }

        int minor_j = 0;
        for(int j = 0; j < arg_matrix->number_of_cols; j++){
            if(j == arg_col){
                continue;
            }
            buffer[minor_i*new_cols + minor_j] = arg_matrix->matrix[i*arg_matrix->number_of_cols + j];
            minor_j++;
        }
        minor_i++;
    }

    return_matrix = init_matrix(new_alias, buffer, new_rows, new_cols);
    double minor_determinant = find_the_determinant(return_matrix);

    free(new_alias);
    free(buffer);
    free(return_matrix);

    return minor_determinant;
}

Matrix* transpose_matrix(const Matrix* arg_matrix){
    double* new_matrix_array = (double*)malloc(arg_matrix->number_of_cols*arg_matrix->number_of_rows*sizeof(double));
    if(new_matrix_array == NULL){
        fprintf(stderr, "Trouble allocating memory for matrix array (%s)!☆\n", __func__);
        exit(1);
    }

    for(int i = 0; i < arg_matrix->number_of_rows; i++){
        for(int j = 0; j < arg_matrix->number_of_cols; j++){
            new_matrix_array[j*arg_matrix->number_of_rows + i] = arg_matrix->matrix[i*arg_matrix->number_of_cols + j];
        }
    }

    char* new_alias = (char*)malloc(sizeof(char)*(strlen(arg_matrix->alias) + 3));
    if(new_alias == NULL){
         fprintf(stderr, "Trouble allocating memory for matrix alias (%s)!☆\n", __func__);
         exit(1);
    }

    memcpy(new_alias, arg_matrix->alias, strlen(arg_matrix->alias));
    memcpy(&new_alias[strlen(arg_matrix->alias)], "^T\0", 3);

    Matrix* new_matrix = init_matrix(new_alias, new_matrix_array, arg_matrix->number_of_cols, arg_matrix->number_of_rows);

    free(new_alias);
    free(new_matrix_array);

    return new_matrix;
}

Matrix* cofactor_matrix(const Matrix* arg_matrix){
    if(arg_matrix->number_of_rows != arg_matrix->number_of_cols){
        fprintf(stderr, "Passing a matrix that isn't square (%s)!☆\n", __func__);
        exit(1);
    }

    Matrix* return_matrix = NULL;
    int new_rows = arg_matrix->number_of_rows, new_cols = arg_matrix->number_of_cols;
    double* buffer = (double*)malloc(sizeof(double)*new_rows*new_cols);
    char* new_alias = (char*)malloc(sizeof(char)*(strlen(arg_matrix->alias) + strlen("_cofactor") + 1));

    memcpy(new_alias, arg_matrix->alias, strlen(arg_matrix->alias));
    memcpy(&new_alias[strlen(arg_matrix->alias)], "_cofactor\0", strlen("_cofactor") + 1);

    for(int i = 0; i < arg_matrix->number_of_rows; i++){
        for(int j = 0; j < arg_matrix->number_of_cols; j++){
            double minor_det = minor_matrix_determinant(arg_matrix, i, j);
            buffer[i*new_cols + j] = pow(-1, i + j) * minor_det;
        }
    }

    return_matrix = init_matrix(new_alias, buffer, new_rows, new_cols);

    free(new_alias);
    free(buffer);

    return return_matrix;
}

Matrix* inverse_matrix(const Matrix* arg_matrix){
    Matrix* c_matrix = cofactor_matrix(arg_matrix);
    Matrix* tc_matrix = transpose_matrix(c_matrix);
    double determinant = find_the_determinant(arg_matrix);
    if(fabs(determinant) < APROX_ZERO){
         fprintf(stderr, "Cannot invert singular matrix (%s)!☆\n", __func__);
         exit(1);
    }

    divide_by_scalar(tc_matrix, determinant);

    free(c_matrix);

    return tc_matrix;
}

void free_matrix(Matrix* arg_matrix){
    if(arg_matrix){
        free(arg_matrix->matrix);
        free(arg_matrix);
    }
}