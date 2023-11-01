#include "Scope.hpp"

Scope::Scope(const string& _name, uint32 _codeBeginPosition, const shared_ptr<Scope> &_parent) {
    name = _name;
    codeBeginPosition = _codeBeginPosition;
    parent = _parent;
}

void Scope::SetDestinationAlias(const string &name, const Token &token) {
    destinationAliases[name] = token;
}

bool Scope::HasDestinationAlias(const string &name) {
    if (parent) {
        if (parent->HasDestinationAlias(name)) {
            return true;
        }
    }

    return destinationAliases.find(name) != destinationAliases.end();
}

Token Scope::GetDestinationAlias(const string &name) {
    auto hasAliasDirect = destinationAliases.find(name) != destinationAliases.end();
    if (hasAliasDirect) {
        return destinationAliases[name];
    }

    return parent ? parent->GetDestinationAlias(name) : Token::MakeUnknown();
}

void Scope::AddChild(const shared_ptr<Scope> &child) {
    children.push_back(child);
}

vector<shared_ptr<Scope>> Scope::GetChildren() {
    return children;
}

shared_ptr<Scope> Scope::GetParent() const {
    return parent;
}

bool Scope::HasDirectChild(const string &name) {
    for (auto &child : children) {
        if (child->GetName() == name) {
            return true;
        }
    }

    return false;
}

shared_ptr<Scope> Scope::GetDirectChild(const string &name) {
    for (auto &child : children) {
        if (child->GetName() == name) {
            return child;
        }
    }

    return nullptr;
}

string Scope::GetName() {
    return name;
}

uint32 Scope::GetCodeBeginPosition() const {
    return codeBeginPosition;
}