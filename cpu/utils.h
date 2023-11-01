#pragma once

#include <stdint.h>
#include <stddef.h>

#define strsimilar(test, value) strcmp(test, value) == 0

struct string_array_t {
    char* value;
    size_t length;
    struct string_array_t* next;
};

struct string_array_t* split_buffer_by_char(const char* buffer, size_t buffer_len, char delimiter);

char* strtrim(const char* source);
char* strslice(const char* source, uint32_t start, uint32_t end);

char* itoa2(int value, int base);