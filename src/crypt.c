#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../headers/crypt.h"
#include "../headers/matrix.h"

typedef union {
  uint32_t words[16]; 
  uint8_t  bytes[64];  
  uint32_t matrix[4][4]; 
} state;

static void load_constant(state *st) 
{
  st->matrix[0][0] = 0x61707865; // "expa"
  st->matrix[0][1] = 0x3320646e; // "nd 3"
  st->matrix[0][2] = 0x79622d32; // "2-by"
  st->matrix[0][3] = 0x6b206574; // "te k"
}

static int load_key(const char *filename, state *st) 
{
  FILE *f = fopen(filename, "rb");
  if (!f) return -1;

  size_t read = fread(&st->matrix[1][0], sizeof(uint32_t), 8, f);
  fclose(f);

  return (read == 8) ? 0 : -2;
}

int generate_nonce(state *st) {
  FILE *urandom = fopen("/dev/urandom", "rb");
  if (!urandom) {
    fprintf(stderr, "File open error: %s\n", __func__);
    return -1;
  }

  uint32_t nonce[3];
  size_t bytes_read = fread(nonce, sizeof(uint32_t), 3, urandom);
  fclose(urandom);

  if (bytes_read != 3) {
    fprintf(stderr, "/dev/urandom read error: %s\n", __func__);
    return -1;
  }

  for (int i = 1; i < 4; ++i)
    st->matrix[3][i] = nonce[i - 1];

  return 0;
}

static int read_nonce(FILE *in, state *st)
{
  if (!in) return -1;

  size_t written = fread(&st->matrix[3][1], 12, 1, in);
  // No need to close file because this is helper function

  return (written == 12) ? 0 : -2;
}

static void quarter_round(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) 
{
  *a += *b; *d ^= *a; *d = (*d << 16) | (*d >> (32 - 16));
  *c += *d; *b ^= *c; *b = (*b << 12) | (*b >> (32 - 12));
  *a += *b; *d ^= *a; *d = (*d << 8) | (*d >> (32 - 8));
  *c += *d; *b ^= *c; *b = (*b << 7) | (*b >> (32 - 7));
}

void generate_key(const char *keyfile)
{
  FILE* urandom = fopen("/dev/urandom", "rb");
  if(!urandom) {
    fprintf(stderr, "File open error: %s\n", __func__);
    fclose(urandom);
    return;
  }

  unsigned char buffer[256];

  size_t bytes_read = fread(buffer, 1, sizeof(buffer), urandom);
  fclose(urandom);

  if (bytes_read != sizeof(buffer)) {
    fprintf(stderr, "/dev/urandom read error: %s", __func__);
    return;
  }

  FILE *key = fopen(keyfile, "wb");
  if (!key) {
    fprintf(stderr, "File open error: %s\n", __func__);
    fclose(key);
    return;
  }

  size_t bytes_written = fwrite(buffer, 1, sizeof(buffer), key);
  fclose(key);

  if (bytes_written != sizeof(buffer)) {
    fprintf(stderr, "Key save to file error %s\n", __func__);
    return;
  }
}

    static state chacha20_block(const state *st) {
  state working_state = *st;
  for (int i = 0; i < 10; i++) {

    for (int col = 0; col < 4; col++) 
      quarter_round(
      &working_state.matrix[0][col], &working_state.matrix[1][col],
      &working_state.matrix[2][col], &working_state.matrix[3][col]);
    

    for (int j = 0; j < 4; j++) 
      quarter_round(&working_state.matrix[0][j],
                    &working_state.matrix[1][(j + 1) % 4],
                    &working_state.matrix[2][(j + 2) % 4],
                    &working_state.matrix[3][(j + 3) % 4]);
  }

  for (int i = 0; i < 16; i++)
    working_state.words[i] += st->words[i];

  return working_state;
}

Buffer* chacha20_encrypt(const char *keyfile, const Matrix* m) 
{
  state st;
  Buffer* buf = serialize_matrix(m);
  Buffer* output = (Buffer*)malloc(sizeof(Buffer));
  if (!output) return NULL;

  output->length = buf->length + 12; 
  output->data = malloc(output->length);
  if (!output->data) {
    free(output);
    return NULL;
  }

  load_constant(&st);
  if (load_key(keyfile, &st) != 0) {
    free(output->data);
    free(output);
    return NULL;
  }

  st.words[12] = 0; // counter
  generate_nonce(&st);

  memcpy(output->data, &st.bytes[52], 12); 

  size_t offset = 0;
  while (offset < buf->length) {
    st.words[12]++;
    state stream = chacha20_block(&st);

    size_t block_size =
        (buf->length - offset > 64) ? 64 : (buf->length - offset);
    for (size_t i = 0; i < block_size; i++) {
      output->data[12 + offset + i] = buf->data[offset + i] ^ stream.bytes[i];
    }

    offset += block_size;
  }

  free(buf);

  return output;
}

Matrix *chacha20_decrypt(const char *keyfile, Buffer *encrypted_buf) 
{
  if (encrypted_buf->length < 12)
    return NULL;

  state st;
  Buffer *output = malloc(sizeof(Buffer));
  if (!output)
    return NULL;

  output->length = encrypted_buf->length - 12;
  output->data = malloc(output->length);
  if (!output->data) {
    free(output);
    return NULL;
  }

  load_constant(&st);
  if (load_key(keyfile, &st) != 0) {
    free(output->data);
    free(output);
    return NULL;
  }

  st.words[12] = 0;
  memcpy(&st.bytes[52], encrypted_buf->data, 12); // прочитаме nonce

  size_t offset = 0;
  while (offset < output->length) {
    st.words[12]++;
    state stream = chacha20_block(&st);

    size_t block_size =
        (output->length - offset > 64) ? 64 : (output->length - offset);
    for (size_t i = 0; i < block_size; i++) {
      output->data[offset + i] =
          encrypted_buf->data[12 + offset + i] ^ stream.bytes[i];
    }

    offset += block_size;
  }

  return deserialize_matrix(output);
}
