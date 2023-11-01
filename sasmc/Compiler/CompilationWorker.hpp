#pragma once

#include "../Common.hpp"
#include "../Parsing/Token.hpp"
#include <vector>
#include <map>

class Scope;
class OpcodeWriter;
class SourceMapWriter;
class Diagnostics;
class CompilationWorker : public enable_shared_from_this<CompilationWorker> {
public:
    CompilationWorker(
        const vector<Token>& tokens,
        const shared_ptr<OpcodeWriter>& writer,
        const shared_ptr<SourceMapWriter>& sourceMapWriter
    );

    void Compile();

    Token CurrentToken();
    Token NextToken();
    Token PeekToken();
    Token EatToken(TokenKind kind);
    void BacktrackToken();
    bool NextTokenIs(TokenKind kind);
    bool IsOutOfBounds();

    int32 GetRegisterIndexOrThrow(const string& name);
    static int32 GetRegisterIndex(const string& name);

    shared_ptr<Diagnostics> GetDiagnostics();

private:
    void CompileToken(const Token& token);
    void CompileIdentifier(const Token& token);
    void CompileFunctionBody();

    void CompileOpSet();
    void CompileOpInc();
    void CompileOpDec();
    void CompileOpJmp();
    void CompileOpJne();
    void CompileOpCmp();
    void CompileOpHlt();
    void CompileOpAdd();
    void CompileOpSub();
    void CompileOpMul();
    void CompileOpDiv();
    void CompileArithmeticOp(Token *outA, Token *outB, Token *outDestination);

    void FillEmptyJmps();

    vector<Token> tokens;
    uint32 tokenIndex;

    shared_ptr<OpcodeWriter> writer;
    shared_ptr<Diagnostics> diagnostics;
    shared_ptr<SourceMapWriter> sourceMapWriter;
    map<uint32, Token> jmpsToFill;
    shared_ptr<Scope> currentScope;
};