#include "Diagnostics.hpp"
#include "../Parsing/Token.hpp"
#include <stdarg.h>

void Diagnostics::ReportSyntaxError(string message, ...) {
    printf("syntax error: ");

    va_list list;
    va_start(list, message);
    vprintf(message.c_str(), list);
    va_end(list);

    printf("\n");

    exit(1);
}

void Diagnostics::ReportSyntaxErrorAt(const Token& token, string message, ...) {
    printf("syntax error: ");

    va_list list;
    va_start(list, message);
    vprintf(message.c_str(), list);
    va_end(list);

    printf(" (at %d:%d)", token.location.line + 1, token.location.column + 1);
    printf("\n");

    exit(1);
}