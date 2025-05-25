#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

static int parse_double(const char* token, double* res) 
{
  char* endptr;

  *res = strtod(token, &endptr);
  
  if (endptr == token || *endptr != '\0') {
    fprintf(stderr, "Invalid number: '%s'\n", token);
    return 0;
  }

  return 1;
}

static int process_line(Matrix* m, char* line) 
{
  static int cols = 0;
  static int initialized = 0;

  int     count  = 0;
  double* values = NULL;

  char *token = strtok(line, " \t\n");

  while (token) {
    double *tmp = realloc(values, sizeof(double) * (count + 1));
    if (!tmp) {
      fprintf(stderr, "Memory allocation error\n");
      free(values);
      return 0;
    }
    values = tmp;
  
    if (!parse_double(token, &values[count])) {
      fprintf(stderr, "Number parsing error\n");
      free(values);
      return 0;
    }
  
    count++;
    token = strtok(NULL, " \t\n");
  }
  

  if (!initialized) {
    cols = count;
    m->number_of_cols = cols;
    initialized = 1;
  } else if (count != cols) {
    fprintf(stderr, "Error: expected %d columns, got %d\n", cols, count);
    free(values);
    return 0;
  }

  double *new_matrix = realloc(
      m->matrix, sizeof(double) * (m->number_of_rows + 1) * m->number_of_cols);
  if (!new_matrix) {
    fprintf(stderr, "Memory allocation failed for matrix data\n");
    free(values);
    return 0;
  }

  m->matrix = new_matrix;

  memcpy(m->matrix + m->number_of_rows * m->number_of_cols, values,sizeof(double) * m->number_of_cols);
  m->number_of_rows++;

  free(values);
  return 1;
}

void print_matrix( Matrix *m) 
{
  for (uint i = 0; i < m->number_of_rows; i++) {
    for (uint j = 0; j < m->number_of_cols; j++) 
      printf("%lf ", m->matrix[i * m->number_of_cols + j]);
    
    printf("\n");
  }

}

Matrix* input_matrix()
{
  // Bypass of init_matrix
  Matrix* m = malloc(sizeof(Matrix));
  m->alias = NULL;
  m->matrix = NULL;
  m->number_of_cols = 0;
  m->number_of_rows = 0;
  
  char buffer[1024];
  
  puts("Enter matrix alias: "); 
  
  if(!fgets(buffer, sizeof(buffer), stdin)){
    fputs("Unexpected input error", stderr);
    return NULL;
  }

  buffer[strlen(buffer) - 1] = '\0';

  m->alias = strdup(buffer);
  if (!m->alias) {
    fprintf(stderr, "Failed to allocate memory for alias\n");
    free(m);
    return NULL;
  }

  puts("Enter matrix (Ctrl+D to end):");

  while (fgets(buffer, sizeof(buffer), stdin)) {
    if (!process_line(m, buffer)) {
      fprintf(stderr, "Error processing line. Stopping input.\n");
      break;
    }
  }

  putchar('\n');

  return m;
}