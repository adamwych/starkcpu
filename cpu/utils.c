#include "utils.h"
#include <stdlib.h>
#include <memory.h>
#include <string.h>

struct string_array_t* split_buffer_by_char(const char* buffer, size_t buffer_len, char delimiter) {
    struct string_array_t* root = malloc(sizeof(struct string_array_t));

    size_t pos = 0;
    size_t line_index = 0;
    size_t line_start_index = 0;
    size_t next_line_start = 0;
    struct string_array_t* previous_element = root;

    while (pos < buffer_len) {
        char ch = buffer[pos++];
        if (ch == delimiter || ch == '\0' || pos == buffer_len) {
            size_t line_length = pos - line_start_index;

            struct string_array_t* element = malloc(sizeof(struct string_array_t));
            element->value = malloc(sizeof(char) * line_length);
            element->length = line_length;

            if (ch == delimiter) {
                line_length -= 1;
            }

            // Don't copy last character which is either the delimiter or NUL, because
            // we want to override it anyway, to make sure that it is always NUL.
            memcpy(element->value, buffer + line_start_index, line_length);

            element->value[line_length] = '\0';
            element->next = 0;

            if (previous_element) {
                previous_element->next = element;
            }

            previous_element = element;

            line_index++;
            line_start_index = pos;
            next_line_start = next_line_start + line_length;
        }
    }

    return root->next;
}

char* strtrim(const char* source) {
    size_t len = strlen(source);
    char* result = malloc(sizeof(char) * len);
    memcpy(result, source, len);

    size_t offset = 0;
    while (result[0] == ' ' && offset < len) {
        result = result + 1;
        offset++;
    }

    result[len - offset] = '\0';

    return result;
}

char* strslice(const char* source, uint32_t start, uint32_t end) {
    uint32_t len = end - start;
    char* result = malloc(sizeof(char) * len);
    memcpy(result, source + start, len);
    result[len] = '\0';
    return result;
}

char* itoa2(int value, int base) {
    char* result = malloc(sizeof(char) * 32);

    // check that the base if valid
    if (base < 2 || base > 36) { *result = '\0'; return result; }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}