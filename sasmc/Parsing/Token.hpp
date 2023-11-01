#pragma once

#include "../Common.hpp"

enum TokenKind {
    Identifier,
    Number,
    Comma,
    SquareBracketOpen,
    SquareBracketClose,
    BracketOpen,
    BracketClose,
    Plus,
    Minus,
    EndOfFile,
    Unknown
};

struct TokenLocation {
    string file;
    uint32 column;
    uint32 line;
};

struct Token {
    string value;
    TokenKind kind;
    TokenLocation location;

    inline int32 ValueAsInt32() {
        return atoi(value.c_str());
    }

    inline bool HasValue(const string& testValue) const {
        return strcmp(value.c_str(), testValue.c_str()) == 0;
    }

    static Token MakeUnknown() {
        return {
            .value = "unknown",
            .kind = TokenKind::Unknown
        };
    }
};

inline string TokenKindToString(TokenKind kind) {
    switch (kind) {
        case TokenKind::Identifier: return "identifier";
        case TokenKind::Number: return "number";
        case TokenKind::Comma: return "comma";
        case TokenKind::BracketOpen: return "bracket open";
        case TokenKind::BracketClose: return "bracket close";
        case TokenKind::SquareBracketOpen: return "square bracket open";
        case TokenKind::SquareBracketClose: return "square bracket close";
        case TokenKind::Plus: return "plus";
        case TokenKind::Minus: return "minus";
        case TokenKind::EndOfFile: return "end of file";
        default: return "unknown";
    }
}