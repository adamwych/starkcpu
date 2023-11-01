#pragma once

#include "../Common.hpp"
#include "Token.hpp"
#include <vector>
#include <functional>

class SourceFile;
class Tokenizer {
public:
    explicit Tokenizer(const shared_ptr<SourceFile>& file);

    vector<Token> Tokenize();

    static vector<Token> TokenizeFile(const shared_ptr<SourceFile>& file);

private:
    Token MakeToken(const string& value, TokenKind kind);
    Token NextToken();
    string ReadTokenIf(const function<bool (char)>& predicate);
    char ReadBuffer();
    bool IsOutOfBounds() const;

    shared_ptr<SourceFile> sourceFile;
    string buffer;
    uint32 bufferLength;
    uint32 position;
    uint32 column;
    uint32 line;
};