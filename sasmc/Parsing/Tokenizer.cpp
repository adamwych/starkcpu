#include "Tokenizer.hpp"
#include "../Compiler/SourceFile.hpp"

Tokenizer::Tokenizer(const shared_ptr<SourceFile> &file) {
    sourceFile = file;
    buffer = file->GetContents();
    bufferLength = buffer.length();
    position = 0;
    column = 0;
    line = 0;
}

vector<Token> Tokenizer::Tokenize() {
    vector<Token> result;

    while (true) {
        auto token = NextToken();

        result.push_back(token);

        if (token.kind == TokenKind::EndOfFile) {
            break;
        }
    }

    return result;
}

Token Tokenizer::MakeToken(const string& value, TokenKind kind) {
    return {
        .value = value,
        .kind = kind,
        .location = {
            .file = sourceFile->GetPath(),
            .column = (uint32)(column - value.length()),
            .line = line
        }
    };
}

Token Tokenizer::NextToken() {
    if (IsOutOfBounds()) {
        return MakeToken("eof", TokenKind::EndOfFile);
    }

    char ch = ReadBuffer();
    if (ch == ' ' || ch == '\t') {
        return NextToken();
    }

    if (ch == '\r' || ch == '\n') {
        line++;
        column = 0;
        return NextToken();
    }

    if (ch == '#') {
        while (!IsOutOfBounds()) {
            ch = ReadBuffer();
            if (ch == '\n') {
                break;
            }
        }

        return NextToken();
    }

    if (isalpha(ch)) {
        auto identifier = ReadTokenIf([](char c) {
            return isalnum(c) || c == '_';
        });

        return MakeToken(identifier, TokenKind::Identifier);
    } else if (isdigit(ch)) {
        auto isHexadecimal = false;
        auto number = ReadTokenIf([&isHexadecimal](char c) {
            if (c == 'x') {
                isHexadecimal = true;
            }

            if (isHexadecimal) {
                return isdigit(c) || c == 'x' || isalpha(c);
            }

            return isdigit(c) || c == 'x';
        });

        // Convert to decimal if provided number is in hexadecimal format.
        if (isHexadecimal) {
            number = to_string(strtol(number.c_str(), nullptr, 16));
        }

        return MakeToken(number, TokenKind::Number);
    }

    switch (ch) {
        case ',': return MakeToken(",", TokenKind::Comma);
        case '[': return MakeToken("[", TokenKind::SquareBracketOpen);
        case ']': return MakeToken("]", TokenKind::SquareBracketClose);
        case '{': return MakeToken("{", TokenKind::BracketOpen);
        case '}': return MakeToken("}", TokenKind::BracketClose);
        case '+': return MakeToken("+", TokenKind::Plus);
        case '-': return MakeToken("-", TokenKind::Minus);
        default: return MakeToken(string(1, ch), TokenKind::Unknown);
    }
}

string Tokenizer::ReadTokenIf(const function<bool (char)>& predicate) {
    column--;
    position--;

    char ch;
    string value;

    while (!IsOutOfBounds()) {
        ch = ReadBuffer();

        if (predicate(ch)) {
            value += ch;
        } else {
            column--;
            position--;
            break;
        }
    }

    return value;
}

char Tokenizer::ReadBuffer() {
    column++;
    return buffer[position++];
}

bool Tokenizer::IsOutOfBounds() const {
    return position >= bufferLength;
}

vector<Token> Tokenizer::TokenizeFile(const shared_ptr<SourceFile> &file) {
    return make_shared<Tokenizer>(file)->Tokenize();
}