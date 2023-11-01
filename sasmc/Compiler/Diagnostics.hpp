#pragma once

#include "../Common.hpp"

struct Token;
class Diagnostics {
public:
    void ReportSyntaxError(string message, ...);
    void ReportSyntaxErrorAt(const Token& token, string message, ...);
};