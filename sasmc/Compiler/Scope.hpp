#pragma once

#include "../Common.hpp"
#include "../Parsing/Token.hpp"
#include <map>
#include <vector>

class Scope {
public:
    Scope(const string& name, uint32 codeBeginPosition, const shared_ptr<Scope>& parent);

    void SetDestinationAlias(const string& name, const Token& token);
    bool HasDestinationAlias(const string& name);
    Token GetDestinationAlias(const string& name);

    void AddChild(const shared_ptr<Scope>& child);
    vector<shared_ptr<Scope>> GetChildren();
    shared_ptr<Scope> GetParent() const;
    bool HasDirectChild(const string& name);
    shared_ptr<Scope> GetDirectChild(const string& name);

    string GetName();
    uint32 GetCodeBeginPosition() const;
private:
    string name;
    uint32 codeBeginPosition;
    shared_ptr<Scope> parent;
    vector<shared_ptr<Scope>> children;
    map<string, Token> destinationAliases;
};