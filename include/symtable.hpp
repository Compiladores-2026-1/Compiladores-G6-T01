#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>

enum class SymbolCategory
{
    VAR,
    FUNC,
    PARAM
};

struct Symbol
{
    std::string name;
    int type_token;
    SymbolCategory category;
    int scope_level;
};

class SymbolTable
{
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;

public:
    SymbolTable();

    void enterScope();
    void exitScope();

    bool insert(const std::string &name, int type_token, SymbolCategory cat);

    Symbol *lookup(const std::string &name);

    Symbol *lookupCurrentScope(const std::string &name);

    void printTable() const;

    static std::string categoryToString(SymbolCategory cat);

    int getCurrentLevel() const { return (int)scopes.size() - 1; }
};
