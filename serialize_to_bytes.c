//
// Created by xinitrix on 13.05.2026.
//
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "serialize_to_bytes.h"

size_t rotate_left(size_t value, char n, char r) {
    n %= r;
    return (value << n) | (value >> (r - n));
}

char to_hex(char b) {
    if(b > 16) return -1;
    if(b < 10) {
        return '0' + b;
    }
    return 'A' + b - 10;
}

char to_dec(char b) {
    if(b >= 'A' && b <= 'F') {
        return b - 'A' + 10;
    }
    if(b >= '0' && b <= '9') {
        return b - '0';
    }
    return -1;
}

char* serialize(const void* key, size_t key_len) {
    if(!key || key_len == 0) return NULL;
    char* out = malloc(key_len*2 + 1);
    if(!out) return NULL;


    size_t strpos = 0;
    const uint8_t* b = (uint8_t*)key;
    for(int i = 0; i < key_len; i+=1) {
        char l = to_hex(b[i] & 0xF);
        char h = to_hex((char)rotate_left(b[i], 4, 8) & 0xF);
        out[strpos++] = h;
        out[strpos++] = l;
    }
    out[strpos] = '\0';

    return out;
}

void* deserialize(const char* key) {
    if(!key) return NULL;
    size_t key_len = strlen(key);
    if (key_len % 2 != 0) return NULL;


    uint8_t* out = malloc(key_len/2);
    if (!out) return NULL;

    uint8_t byte = 0;
    size_t strpos = 0;
    for(size_t i = 0; i < key_len / 2; i+=1) {
        char h = to_dec(key[strpos++]);
        char l = to_dec(key[strpos++]);

        if (h == -1 || l == -1) {
            free(out);
            return NULL;
        }

        byte = (uint8_t)h;
        byte = rotate_left(byte, 4, 8) + (uint8_t)l;
        out[i] = byte;
    }
    return (void*)out;
}

