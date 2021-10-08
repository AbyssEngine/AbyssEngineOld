#ifndef LIBABYSS_CRYPTO_H
#define LIBABYSS_CRYPTO_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void crypto_initialize();
uint32_t crypto_lookup(uint32_t index);
void crypto_decrypt(uint32_t* data, size_t data_length,  uint32_t seed);
void crypto_decrypt_bytes(char* data, size_t data_length, uint32_t seed);
uint32_t crypto_hash_string(const char *key, uint32_t hash_type);
uint32_t *crypto_decrypt_table(FILE *source, uint32_t size, const char *name);
#endif // LIBABYSS_CRYPTO_H
