#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "matrix.h"

Matrix* init_matrix(char* arg_alias, double* arg_matrix_array, uint arg_number_of_rows, uint arg_number_of_cols){
    Matrix* temp_matrix = (Matrix*)malloc(sizeof(Matrix));
    if(temp_matrix == NULL){
        printf("Trouble allocating memory for matrix (init_matrix)!☆\n");
        exit(1);
    }

    temp_matrix->alias = malloc(strlen(arg_alias) + 1);
    if (temp_matrix->alias == NULL){
        printf("Trouble allocating memory for alias!☆\n");
        exit(1);
    }
    memcpy(temp_matrix->alias, arg_alias, strlen(arg_alias) + 1);

    if(arg_number_of_rows <= 0 || arg_number_of_cols <= 0){
        printf("Invalid argument (init_matrix)!☆\n");
        exit(1);
    }
    temp_matrix->number_of_rows = arg_number_of_rows;
    temp_matrix->number_of_cols = arg_number_of_cols;

    temp_matrix->matrix = (double*)malloc(sizeof(double)*temp_matrix->number_of_cols*temp_matrix->number_of_rows);
    if(temp_matrix->matrix == NULL){
        printf("Trouble allocating memory for matrix array (init_matrix)!☆\n");
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
        printf("Cannot multiply matrices (multiply_by_matrix)!☆\n");
        exit(1);
    }

    Matrix* new_matrix = (Matrix*)malloc(sizeof(Matrix));
    if(new_matrix == NULL){
        printf("Trouble allocating memory for matrix (multiply_by_matrix)!☆\n");
        exit(1);
    }

    new_matrix->number_of_rows =  arg_matrix_1->number_of_rows;
    new_matrix->number_of_cols = arg_matrix_2->number_of_cols;

    new_matrix->matrix = (double*)malloc(sizeof(double)*new_matrix->number_of_rows*new_matrix->number_of_cols);
    if(new_matrix->matrix == NULL){
        printf("Trouble allocating memory for matrix array (multiply_by_matrix)!☆\n");
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
        printf("Trouble allocating memory for matrix alias (multiply_by_matrix)!☆\n");
        exit(1);
    }
    memcpy(new_matrix->alias, arg_matrix_1->alias, len1);
    new_matrix->alias[len1] = 'x';
    memcpy(&new_matrix->alias[len1 + 1], arg_matrix_2->alias, len2 + 1);

    return new_matrix;
}

Matrix* divide_by_scalar(Matrix* arg_matrix, double arg_scalar){

}

Matrix* inverse_matrix(Matrix* arg_matrix){

}

Matrix* transpose_matrix(Matrix* arg_matrix){

}

double find_the_determinant(Matrix* arg_matrix){

}

void print_matrix(Matrix*){

}

void free_matrix(Matrix*){

}

/* Push attempt */
