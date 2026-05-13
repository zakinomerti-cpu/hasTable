//
// Created by xinitrix on 13.05.2026.
//

#ifndef NJAKO_SERIALIZE_TO_BYTES_H
#define NJAKO_SERIALIZE_TO_BYTES_H

#include <stddef.h>

size_t rotate_left(size_t value, char n, char r);
char to_hex(char b);
char to_dec(char b);

char* serialize(const void* key, size_t key_len);
void* deserialize(const char* key);

#endif //NJAKO_SERIALIZE_TO_BYTES_H
