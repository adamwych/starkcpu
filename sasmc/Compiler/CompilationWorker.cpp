#include "CompilationWorker.hpp"
#include "Diagnostics.hpp"
#include "OpcodeWriter.hpp"
#include "Scope.hpp"
#include "SourceMapWriter.hpp"

#define MEMORY_CODE_OFFSET 0x00000100

CompilationWorker::CompilationWorker(
    const vector<Token> &_tokens,
    const shared_ptr<OpcodeWriter>& _writer,
    const shared_ptr<SourceMapWriter>& _sourceMapWriter
) {
    tokens = _tokens;
    tokenIndex = 0;
    writer = _writer;
    sourceMapWriter = _sourceMapWriter;
    diagnostics = make_shared<Diagnostics>();
}

void CompilationWorker::Compile() {
    currentScope = make_shared<Scope>("", 0, nullptr);

    while (!IsOutOfBounds()) {
        auto token = NextToken();
        if (token.kind == TokenKind::EndOfFile) {
            break;
        }

        CompileToken(token);
    }

    FillEmptyJmps();
}

void CompilationWorker::FillEmptyJmps() {
    for (auto &entry : jmpsToFill) {
        auto name = entry.second.value;
        if (currentScope->HasDirectChild(name)) {
            writer->ReplaceInt32(entry.first + 1, MEMORY_CODE_OFFSET + currentScope->GetDirectChild(name)->GetCodeBeginPosition());
        } else {
            diagnostics->ReportSyntaxErrorAt(entry.second, "cannot jmp to '%s', because it does not exist in current context", name.c_str());
        }
    }
}

void CompilationWorker::CompileToken(const Token& token) {
    switch (token.kind) {
        case TokenKind::Identifier:
            CompileIdentifier(token);
            break;

        default:
            break;
    }
}

void CompilationWorker::CompileIdentifier(const Token& token) {
    if (NextTokenIs(TokenKind::BracketOpen)) {
        EatToken(TokenKind::BracketOpen);

        auto scope = make_shared<Scope>(token.value, writer->GetPosition(), currentScope);
        currentScope->AddChild(scope);
        currentScope = scope;

        CompileFunctionBody();

        currentScope = currentScope->GetParent();
        EatToken(TokenKind::BracketClose);
    } else {
        sourceMapWriter->Write(writer->GetPosition(), token.location.line);

        if (token.HasValue("set")) {
            CompileOpSet();
        } else if (token.HasValue("inc")) {
            CompileOpInc();
        } else if (token.HasValue("dec")) {
            CompileOpDec();
        } else if (token.HasValue("jmp")) {
            CompileOpJmp();
        } else if (token.HasValue("jne")) {
            CompileOpJne();
        } else if (token.HasValue("cmp")) {
            CompileOpCmp();
        } else if (token.HasValue("hlt")) {
            CompileOpHlt();
        } else if (token.HasValue("add")) {
            CompileOpAdd();
        } else if (token.HasValue("sub")) {
            CompileOpSub();
        } else if (token.HasValue("mul")) {
            CompileOpMul();
        } else if (token.HasValue("div")) {
            CompileOpDiv();
        } else {
            diagnostics->ReportSyntaxErrorAt(token, "unknown token %s", token.value.c_str());
        }
    }
}

void CompilationWorker::CompileFunctionBody() {
    while (!IsOutOfBounds()) {
        auto token = NextToken();
        if (token.kind != TokenKind::BracketClose) {
            CompileToken(token);
        } else {
            BacktrackToken();
            break;
        }
    }
}

void CompilationWorker::CompileOpSet() {
    auto destinationToken = NextToken();
    auto isDestinationAddressRef = false;
    if (destinationToken.kind == TokenKind::SquareBracketOpen) {
        isDestinationAddressRef = true;
        destinationToken = NextToken();
        EatToken(TokenKind::SquareBracketClose);
    }

    if (destinationToken.kind != TokenKind::Identifier && destinationToken.kind != TokenKind::Number) {
        diagnostics->ReportSyntaxErrorAt(destinationToken, "`set` expects first operand to be an identifier, address or address reference");
    }

    EatToken(TokenKind::Comma);

    auto sourceToken = NextToken();
    auto isSourceAddressRef = false;
    if (sourceToken.kind == TokenKind::SquareBracketOpen) {
        isSourceAddressRef = true;
        sourceToken = NextToken();
        EatToken(TokenKind::SquareBracketClose);
    }

    if (destinationToken.kind == TokenKind::Identifier) {
        if (currentScope->HasDestinationAlias(destinationToken.value)) {
            destinationToken = currentScope->GetDestinationAlias(destinationToken.value);
        }
    }

    if (sourceToken.kind == TokenKind::Identifier) {
        if (currentScope->HasDestinationAlias(sourceToken.value)) {
            sourceToken = currentScope->GetDestinationAlias(sourceToken.value);
        }
    }

    if (destinationToken.kind == TokenKind::Identifier) {
        if (sourceToken.kind == TokenKind::Identifier) {
            if (isSourceAddressRef && isDestinationAddressRef) {
                writer->WriteSetRAddrRAddr(
                    GetRegisterIndexOrThrow(destinationToken.value),
                    GetRegisterIndexOrThrow(sourceToken.value)
                );
            } else {
                writer->WriteSetRegReg(
                    GetRegisterIndexOrThrow(destinationToken.value),
                    GetRegisterIndexOrThrow(sourceToken.value)
                );
            }
        } else {
            if (isDestinationAddressRef && !isSourceAddressRef) {
                writer->WriteSetRAddrImmediate(
                    GetRegisterIndexOrThrow(destinationToken.value),
                    sourceToken.ValueAsInt32()
                );
            } else if (!isDestinationAddressRef && isSourceAddressRef) {
                writer->WriteSetRegAddr(
                    GetRegisterIndexOrThrow(destinationToken.value),
                    sourceToken.ValueAsInt32()
                );
            } else if (!isDestinationAddressRef && !isSourceAddressRef) {
                writer->WriteSetRegImmediate(
                    GetRegisterIndexOrThrow(destinationToken.value),
                    sourceToken.ValueAsInt32()
                );
            }
        }
    } else if (destinationToken.kind == TokenKind::Number) {
        if (sourceToken.kind == TokenKind::Identifier) {
            writer->WriteSetAddrReg(destinationToken.ValueAsInt32(), GetRegisterIndexOrThrow(sourceToken.value));
        } else {
            if (isSourceAddressRef) {
                // @todo
            } else {
                writer->WriteSetAddrImmediate(destinationToken.ValueAsInt32(), sourceToken.ValueAsInt32());
            }
        }
    }

    auto nextToken = PeekToken();
    if (nextToken.kind == TokenKind::Identifier && nextToken.HasValue("as")) {
        NextToken();
        auto aliasToken = EatToken(TokenKind::Identifier);

        if (currentScope->HasDestinationAlias(aliasToken.value)) {
            diagnostics->ReportSyntaxErrorAt(aliasToken, "alias %s is already in use", aliasToken.value.c_str());
        }

        currentScope->SetDestinationAlias(aliasToken.value, destinationToken);
    }
}

void CompilationWorker::CompileOpInc() {
    auto targetToken = EatToken(TokenKind::Identifier);
    if (currentScope->HasDestinationAlias(targetToken.value)) {
        targetToken = currentScope->GetDestinationAlias(targetToken.value);
    }

    writer->WriteIncReg(GetRegisterIndex(targetToken.value));
}

void CompilationWorker::CompileOpDec() {
    auto targetToken = EatToken(TokenKind::Identifier);
    if (currentScope->HasDestinationAlias(targetToken.value)) {
        targetToken = currentScope->GetDestinationAlias(targetToken.value);
    }

    writer->WriteDecReg(GetRegisterIndex(targetToken.value));
}

void CompilationWorker::CompileOpJmp() {
    auto destinationToken = NextToken();

    if (destinationToken.kind == TokenKind::SquareBracketOpen) {
        destinationToken = NextToken();
        EatToken(TokenKind::SquareBracketClose);
    }

    if (destinationToken.kind == TokenKind::Identifier) {
        if (currentScope->HasDestinationAlias(destinationToken.value)) {
            destinationToken = currentScope->GetDestinationAlias(destinationToken.value);
        }

        auto registerIndex = GetRegisterIndex(destinationToken.value);
        if (registerIndex != -1) {
            writer->WriteJmpReg(registerIndex);
        } else {
            if (currentScope->GetName() == destinationToken.value) {
                writer->WriteJmp(MEMORY_CODE_OFFSET + currentScope->GetCodeBeginPosition());
            } else if (currentScope->HasDirectChild(destinationToken.value)) {
                writer->WriteJmp(MEMORY_CODE_OFFSET + currentScope->GetDirectChild(destinationToken.value)->GetCodeBeginPosition());
            } else {
                auto start = writer->GetPosition();
                writer->WriteJmp(0);
                jmpsToFill[start] = destinationToken;
            }
        }
    } else if (destinationToken.kind == TokenKind::Number) {
        writer->WriteJmp(destinationToken.ValueAsInt32());
    } else {
        diagnostics->ReportSyntaxErrorAt(destinationToken, "jmp expects operand to be the name of a scope or an address");
    }
}

void CompilationWorker::CompileOpJne() {
    auto destinationToken = NextToken();
    if (destinationToken.kind == TokenKind::Identifier) {
        if (currentScope->GetName() == destinationToken.value) {
            writer->WriteJne(MEMORY_CODE_OFFSET + currentScope->GetCodeBeginPosition());
        } else if (currentScope->HasDirectChild(destinationToken.value)) {
            writer->WriteJne(MEMORY_CODE_OFFSET + currentScope->GetDirectChild(destinationToken.value)->GetCodeBeginPosition());
        } else {
            auto start = writer->GetPosition();
            writer->WriteJne(0);
            jmpsToFill[start] = destinationToken;
        }
    } else if (destinationToken.kind == TokenKind::Number) {
        writer->WriteJne(destinationToken.ValueAsInt32());
    } else {
        diagnostics->ReportSyntaxErrorAt(destinationToken, "jmp expects operand to be the name of a scope or an address");
    }
}

void CompilationWorker::CompileOpCmp() {
    auto sourceToken = NextToken();
    auto isSourceAddressRef = false;

    if (sourceToken.kind == TokenKind::SquareBracketOpen) {
        sourceToken = NextToken();
        EatToken(TokenKind::SquareBracketClose);
        isSourceAddressRef = true;
    } else if (sourceToken.kind == TokenKind::Number) {
        diagnostics->ReportSyntaxErrorAt(sourceToken, "cmp expects first operand to be a register or memory reference");
    }

    if (currentScope->HasDestinationAlias(sourceToken.value)) {
        sourceToken = currentScope->GetDestinationAlias(sourceToken.value);
    }

    EatToken(TokenKind::Comma);
    auto valueToken = EatToken(TokenKind::Number);

    if (isSourceAddressRef) {
        // @todo
    } else {
        writer->WriteCmpRegImmediate(GetRegisterIndexOrThrow(sourceToken.value), valueToken.ValueAsInt32());
    }
}

void CompilationWorker::CompileOpHlt() {
    writer->WriteHalt();
}

void CompilationWorker::CompileOpAdd() {
    Token aToken, bToken, destinationToken;
    CompileArithmeticOp(&aToken, &bToken, &destinationToken);

    if (aToken.kind != TokenKind::Identifier) {
        diagnostics->ReportSyntaxErrorAt(aToken, "add expects first operand to be a register");
    }

    if (bToken.kind == TokenKind::Identifier) {
        writer->WriteAddRegRegReg(GetRegisterIndexOrThrow(aToken.value), GetRegisterIndexOrThrow(bToken.value),
                                  GetRegisterIndexOrThrow(destinationToken.value));
    } else if (bToken.kind == TokenKind::Number) {
        writer->WriteAddRegImmReg(GetRegisterIndexOrThrow(aToken.value), bToken.ValueAsInt32(),
                                  GetRegisterIndexOrThrow(destinationToken.value));
    } else {
        diagnostics->ReportSyntaxErrorAt(bToken, "add expects second operand to be a register or a number");
    }
}

void CompilationWorker::CompileOpSub() {
    Token aToken, bToken, destinationToken;
    CompileArithmeticOp(&aToken, &bToken, &destinationToken);

    if (aToken.kind == TokenKind::Identifier) {
        if (bToken.kind == TokenKind::Identifier) {
            writer->WriteSubRegRegReg(GetRegisterIndexOrThrow(aToken.value), GetRegisterIndexOrThrow(bToken.value),
                                      GetRegisterIndexOrThrow(destinationToken.value));
        } else if (bToken.kind == TokenKind::Number) {
            writer->WriteSubRegImmReg(GetRegisterIndexOrThrow(aToken.value), bToken.ValueAsInt32(),
                                      GetRegisterIndexOrThrow(destinationToken.value));
        } else {
            diagnostics->ReportSyntaxErrorAt(bToken, "sub expected second operand to be a register or a number");
        }
    } else if (aToken.kind == TokenKind::Number) {
        if (bToken.kind == TokenKind::Identifier) {
            writer->WriteSubImmRegReg(GetRegisterIndexOrThrow(bToken.value), aToken.ValueAsInt32(),
                                      GetRegisterIndexOrThrow(destinationToken.value));
        } else {
            diagnostics->ReportSyntaxErrorAt(bToken, "sub expected second operand to be a register");
        }
    } else {
        diagnostics->ReportSyntaxErrorAt(aToken, "sub expects first operand to be a register or a number");
    }
}

void CompilationWorker::CompileOpMul() {
    Token aToken, bToken, destinationToken;
    CompileArithmeticOp(&aToken, &bToken, &destinationToken);

    if (aToken.kind != TokenKind::Identifier) {
        diagnostics->ReportSyntaxErrorAt(aToken, "mul expects first operand to be a register");
    }

    if (bToken.kind == TokenKind::Identifier) {
        writer->WriteMulRegRegReg(GetRegisterIndexOrThrow(aToken.value), GetRegisterIndexOrThrow(bToken.value),
                                  GetRegisterIndexOrThrow(destinationToken.value));
    } else if (bToken.kind == TokenKind::Number) {
        writer->WriteMulRegImmReg(GetRegisterIndexOrThrow(aToken.value), bToken.ValueAsInt32(),
                                  GetRegisterIndexOrThrow(destinationToken.value));
    } else {
        diagnostics->ReportSyntaxErrorAt(bToken, "mul expects second operand to be a register or a number");
    }
}

void CompilationWorker::CompileOpDiv() {
    Token aToken, bToken, destinationToken;
    CompileArithmeticOp(&aToken, &bToken, &destinationToken);

    if (aToken.kind == TokenKind::Identifier) {
        if (bToken.kind == TokenKind::Identifier) {
            writer->WriteDivRegRegReg(GetRegisterIndexOrThrow(aToken.value), GetRegisterIndexOrThrow(bToken.value),
                                      GetRegisterIndexOrThrow(destinationToken.value));
        } else if (bToken.kind == TokenKind::Number) {
            writer->WriteDivRegImmReg(GetRegisterIndexOrThrow(aToken.value), bToken.ValueAsInt32(),
                                      GetRegisterIndexOrThrow(destinationToken.value));
        } else {
            diagnostics->ReportSyntaxErrorAt(bToken, "div expected second operand to be a register or a number");
        }
    } else if (aToken.kind == TokenKind::Number) {
        if (bToken.kind == TokenKind::Identifier) {
            writer->WriteDivImmRegReg(GetRegisterIndexOrThrow(bToken.value), aToken.ValueAsInt32(),
                                      GetRegisterIndexOrThrow(destinationToken.value));
        } else {
            diagnostics->ReportSyntaxErrorAt(bToken, "div expected second operand to be a register");
        }
    } else {
        diagnostics->ReportSyntaxErrorAt(aToken, "div expects first operand to be a register or a number");
    }
}

void CompilationWorker::CompileArithmeticOp(Token *outA, Token *outB, Token *outDestination) {
    auto firstValueToken = NextToken();
    EatToken(TokenKind::Comma);
    auto secondValueToken = NextToken();
    EatToken(TokenKind::Comma);
    auto destinationToken = EatToken(TokenKind::Identifier);

    if (firstValueToken.kind == TokenKind::Identifier) {
        if (currentScope->HasDestinationAlias(firstValueToken.value)) {
            firstValueToken = currentScope->GetDestinationAlias(firstValueToken.value);
        }
    }

    if (secondValueToken.kind == TokenKind::Identifier) {
        if (currentScope->HasDestinationAlias(secondValueToken.value)) {
            secondValueToken = currentScope->GetDestinationAlias(secondValueToken.value);
        }
    }

    if (currentScope->HasDestinationAlias(destinationToken.value)) {
        destinationToken = currentScope->GetDestinationAlias(destinationToken.value);
    }

    *outA = firstValueToken;
    *outB = secondValueToken;
    *outDestination = destinationToken;
}

Token CompilationWorker::CurrentToken() {
    return tokens[tokenIndex - 1];
}

Token CompilationWorker::NextToken() {
    return tokens[tokenIndex++];
}

Token CompilationWorker::PeekToken() {
    return tokens[tokenIndex];
}

Token CompilationWorker::EatToken(TokenKind kind) {
    auto nextToken = PeekToken();
    if (nextToken.kind == kind) {
        return NextToken();
    } else {
        diagnostics->ReportSyntaxErrorAt(nextToken, "unexpected token %s (%s), expected %s", nextToken.value.c_str(), TokenKindToString(nextToken.kind).c_str(),
                         TokenKindToString(kind).c_str());
    }

    return nextToken;
}

bool CompilationWorker::NextTokenIs(TokenKind kind) {
    return PeekToken().kind == kind;
}

void CompilationWorker::BacktrackToken() {
    tokenIndex--;
}

bool CompilationWorker::IsOutOfBounds() {
    return tokenIndex >= tokens.size() - 1;
}

int32 CompilationWorker::GetRegisterIndexOrThrow(const string &name) {
    auto result = GetRegisterIndex(name);
    if (result == -1) {
        diagnostics->ReportSyntaxError("unknown register %s", name.c_str());
    }

    return result;
}

int32 CompilationWorker::GetRegisterIndex(const string &name) {
    if (name == "r0") {
        return 0;
    } else if (name == "r1") {
        return 1;
    } else if (name == "r2") {
        return 2;
    }

    return -1;
}

shared_ptr<Diagnostics> CompilationWorker::GetDiagnostics() {
    return diagnostics;
}