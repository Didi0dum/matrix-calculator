#ifndef CRYPT_H
#define CRYPT_H

#include <stdio.h>
#include <stdint.h> 

#include "io.h"
#include "matrix.h"

typedef struct state State; // opaque struct

void    generate_key    (const char* keyfile);

Buffer* chacha20_encrypt(const char* keyfile, const Matrix* m);
Matrix* chacha20_decrypt(const char* keyfile, Buffer* encrypted_buf);

#endif /* CRYPT_H */
