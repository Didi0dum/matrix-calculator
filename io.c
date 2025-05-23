#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "matrix.h"

#define MAX_LINE_LENGTH 1024
#define MAX_LINES 100

void print_matrix(Matrix *matrix) {
  for (int i = 0; i < matrix->number_of_rows; ++i) {
    for (int j = 0; j < matrix->number_of_cols; ++j)
      printf("%lf ", matrix->matrix[i * matrix->number_of_rows + j]);
    putchar('\n');
  }
}

// Multiline input
void read_lines_until_eof(char *lines[], int max_lines) {
  char buffer[MAX_LINE_LENGTH];
  int count = 0;

  puts("Enter multiple lines of text (Press Ctrl+D to finish on Unix):");

  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    if (count >= max_lines) {
      fprintf(stderr, "Line limit reached (%d)\n", max_lines);
      break;
    }

    buffer[strcspn(buffer, "\n")] = '\0'; 

    lines[count] = strdup(buffer); 
    if (lines[count] == NULL) {
      fprintf(stderr, "Memory allocation failed.\n");
      break;
    }

    count++;
  }
}


