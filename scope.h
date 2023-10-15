#ifndef SCOPE_H
#define SCOPE_H
#include <iostream>
#include <string>
#include <map>
#include <stack>
#include "tokens.h"

struct Symbol {
    std::string name;
    t_token type; 
};

struct Scope {
    std::map<std::string, Symbol> symbols;
    Scope* parentScope; // escopo pai (para escopos aninhados)

    // Construtor padrão
    Scope() : parentScope(nullptr) {}

    // Construtor que aceita um escopo pai
    Scope(Scope* parent) : parentScope(parent) {}
};

std::stack<Scope*> scopeStack;

void declareVariable(const std::string& name, t_token type) {
    Scope* currentScope = scopeStack.top();
    if (currentScope->symbols.find(name) != currentScope->symbols.end()) {
        std::cerr << "Erro: Variável " << name << " já declarada no escopo atual." << std::endl;
    } else {
        Symbol newSymbol;
        newSymbol.name = name;
        newSymbol.type = type;
        currentScope->symbols[name] = newSymbol;
    }
}

bool variableDeclared(const std::string& name) {
    Scope* currentScope = scopeStack.top();
    while (currentScope) {
        if (currentScope->symbols.find(name) != currentScope->symbols.end()) {
            return true; // variável encontrada em algum escopo
        }
        currentScope = currentScope->parentScope;
    }
    return false; // variável não declarada em nenhum escopo
}

void checkVariableDeclaration(const std::string& name, t_token type) {
    if (variableDeclared(name)) {
        std::cerr << "Erro: Variável " << name << " já declarada." << std::endl;
    } else {
        declareVariable(name, type);
    }
}

void checkVariableUsage(const std::string& name) {
    if (!variableDeclared(name)) {
        std::cerr << "Erro: Variável " << name << " não foi declarada." << std::endl;
    }
}

void enterNewScope() {
    Scope* newScope = new Scope();
    newScope->parentScope = scopeStack.top();
    scopeStack.push(newScope);
}

void leaveScope() {
    Scope* topScope = scopeStack.top();
    scopeStack.pop();
    delete topScope; // Libere a memória do escopo que está saindo
}

#endif