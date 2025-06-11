#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../headers/io.h"
#include "../headers/crypt.h"
#include "../headers/matrix.h"


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
    free(m);
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

void print_matrix(Matrix *m) 
{
  printf("%s:\n", m->alias);
  for (uint i = 0; i < m->number_of_rows; i++) {
    for (uint j = 0; j < m->number_of_cols; j++) 
      printf("%lf ", m->matrix[i * m->number_of_cols + j]);
    
    printf("\n");
  }
}

Buffer* serialize_matrix(const Matrix *m) 
{
  if (!m) return NULL; 

  uint32_t alias_len  = strlen(m->alias);
  uint32_t total_size = sizeof(uint32_t) +    // Size of alias
                        alias_len +           // Alias
                        sizeof(uint32_t) +    // Cols
                        sizeof(uint32_t) +    // Rows
                        sizeof(double) * m->number_of_cols * m->number_of_rows; // Matrix

  unsigned char *buf_data = malloc(total_size);
  if (!buf_data) return NULL; 

  unsigned char *p = buf_data;
  memcpy(p, &alias_len, sizeof(uint32_t));           p += sizeof(uint32_t);
  memcpy(p, m->alias, alias_len);                    p += alias_len;
  memcpy(p, &m->number_of_cols, sizeof(uint32_t));   p += sizeof(uint32_t);
  memcpy(p, &m->number_of_rows, sizeof(uint32_t));   p += sizeof(uint32_t);
  memcpy(p, m->matrix, sizeof(double) * m->number_of_cols * m->number_of_rows);

  Buffer *buf = malloc(sizeof(Buffer));
  if (!buf) {
    free(buf_data);
    return NULL;
  }

  buf->data = buf_data;
  buf->length = total_size;

  return buf;
}

Matrix *deserialize_matrix(const Buffer *buf) 
{
  if (!buf || !buf->data || buf->length < sizeof(uint32_t) * 3){
    fprintf(stderr, "Incorrect buffer: %s\n", __func__);
    return NULL;
  }

  const unsigned char *p = buf->data;

  uint32_t alias_len;
  memcpy(&alias_len, p, sizeof(uint32_t));
  p += sizeof(uint32_t);

  if (buf->length < sizeof(uint32_t) + alias_len + 2 * sizeof(uint32_t)){
    fprintf(stderr, "Incorrect buffer length: %s\n", __func__);
    return NULL;
  }

  Matrix *m = malloc(sizeof(Matrix));
  if (!m){
    fprintf(stderr, "Memory for matrix allocation error: %s\n", __func__);
    free(m);
    return NULL;
  }

  m->alias = malloc(alias_len + 1);
  if (!m->alias) {
    fprintf(stderr, "Memory for alias allocation error: %s\n", __func__);
    free(m);
    return NULL;
  }
  memcpy(m->alias, p, alias_len);
  m->alias[alias_len] = '\0';
  p += alias_len;

  memcpy(&m->number_of_cols, p, sizeof(uint32_t));
  p += sizeof(uint32_t);
  memcpy(&m->number_of_rows, p, sizeof(uint32_t));
  p += sizeof(uint32_t);

  size_t matrix_size = sizeof(double) * m->number_of_cols * m->number_of_rows;
  if ((size_t)(p - buf->data) + matrix_size > buf->length) {
    fprintf(stderr, "Buffer length: %s\n", __func__);
    free(m->alias);
    free(m);
    return NULL;
  }

  m->matrix = malloc(matrix_size);
  if (!m->matrix) {
    fprintf(stderr, "Memory for matrix allocation error: %s\n", __func__);
    free(m->alias);
    free(m);
    return NULL;
  }

  memcpy(m->matrix, p, matrix_size);
  return m;
}

void save_matrix(const Matrix* m, const char* filename)
{
  Buffer* buf = serialize_matrix(m);

  FILE* f = fopen(filename, "wb");
  if(!f) {
    fprintf(stderr, "File open error: %s", __func__);
    fclose(f);
    return;
  }

  if (fwrite(buf->data, sizeof(unsigned char), buf->length, f) != buf->length) {
    fprintf(stderr, "File write error: %s\n", __func__);
    fclose(f);
    return;
  }

  free(buf);
  fclose(f);
}

void save_matrix_enctypted(const Matrix* m, const char* filename, const char* keyfile)
{
  Buffer* buf = chacha20_encrypt(keyfile, m);

  FILE* f = fopen(filename, "wb");
  if(!f) {
    fprintf(stderr, "File open error: %s", __func__);
    fclose(f);
    return;
  }

  if (fwrite(buf->data, sizeof(unsigned char), buf->length, f) != buf->length) {
    fprintf(stderr, "File write error: %s\n", __func__);
    fclose(f);
    return;
  }

  free(buf->data);
  free(buf);
  fclose(f);
}

Buffer* load_to_buffer(const char* filename) 
{
  FILE* f = fopen(filename, "rb");
  if (!f) {
    fprintf(stderr, "File open error: %s\n", __func__);
    return NULL;
  }

  if (fseek(f, 0, SEEK_END) != 0) {
    fprintf(stderr, "fseek error: %s\n", __func__);
    fclose(f);
    return NULL;
  }

  long fileSize = ftell(f);
  if (fileSize < 0) {
    fprintf(stderr, "ftell error: %s\n", __func__);
    fclose(f);
    return NULL;
  }

  rewind(f);

  Buffer* buf = (Buffer*) malloc(sizeof(Buffer));
  if (!buf) {
    fprintf(stderr, "Memory allocation error: %s\n", __func__);
    fclose(f);
    return NULL;
  }

  buf->data = (unsigned char*) malloc(fileSize);
  if (!buf->data) {
    fprintf(stderr, "Buffer memory allocation failed: %s\n", __func__);
    free(buf);
    fclose(f);
    return NULL;
  }

  size_t bytesRead = fread(buf->data, 1, fileSize, f);
  if (bytesRead != (size_t)fileSize) {
    fprintf(stderr, "Incomplete read from file: %s\n", __func__);
    free(buf->data);
    free(buf);
    fclose(f);
    return NULL;
  }

  buf->length = bytesRead;
  fclose(f);
  return buf;
}

Matrix* load_matrix(const char* filename) 
{
  Buffer* buf = load_to_buffer(filename);
  if (!buf) return NULL;

  Matrix* m = deserialize_matrix(buf);
  free(buf->data);
  free(buf);
  return m;
}

Matrix* load_matrix_encrypted(const char* filename, const char* keyfile) {
  Buffer* buf = load_to_buffer(filename);
  if (!buf) return NULL;

  Matrix* m = chacha20_decrypt(keyfile, buf);
  free(buf->data);
  free(buf);
  return m;
}

