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

Matrix* multiply_by_matrix(Matrix* arg_matrix_1, Matrix* arg_matrix_2){
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

Matrix* divide_by_scalar(Matrix* arg_matrix, double arg_scalar){
     for(int i = 0; i < arg_matrix->number_of_rows; i++){
        for(int j = 0; j < arg_matrix->number_of_cols; j++){
            arg_matrix->matrix[i*arg_matrix->number_of_cols + j] /= arg_scalar;
        }
    }
}

Matrix* inverse_matrix(Matrix* arg_matrix){
    fprintf(stderr, "Not implemented: %s\n", __func__);
    return NULL;
}

Matrix* transpose_matrix(Matrix* arg_matrix){
    fprintf(stderr, "Not implemented: %s\n", __func__);
    return NULL;
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

double find_the_determinant(Matrix* arg_matrix){
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

void free_matrix(Matrix* arg_matrix){
    free(arg_matrix->matrix);
    free(arg_matrix);
}