#ifndef IO_H
#define IO_H

#include "stdlib.h"
#include "matrix.h"

typedef struct {
  unsigned char *data;
  size_t length;
} Buffer;

Matrix* input_matrix();
void    print_matrix(Matrix*m);


Buffer* serialize_matrix       (const Matrix* m);
Matrix* deserialize_matrix     (const Buffer* buf);

void    save_matrix            (const Matrix* m, const char* filename);
void    save_matrix_enctypted  (const Matrix* m, const char* filename, const char* keyfile);
Matrix* load_matrix            (const char* filename);
Matrix* load_matrix_encrypted  (const char* filename, const char* keyfile);

#endif /* IO_H */