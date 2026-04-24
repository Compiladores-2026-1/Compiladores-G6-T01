#include "symtable.hpp"
#include <iomanip>

SymbolTable::SymbolTable()
{
    enterScope();
}

void SymbolTable::enterScope()
{
    scopes.emplace_back(std::unordered_map<std::string, Symbol>());
}

void SymbolTable::exitScope()
{
    if (scopes.size() > 1)
    {
        scopes.pop_back();
    }
    else
    {
        std::cerr << "Aviso: Tentativa de remover o escopo global ignorada.\n";
    }
}

bool SymbolTable::insert(const std::string &name, int type_token, SymbolCategory cat)
{
    auto &current_scope = scopes.back();

    if (current_scope.find(name) != current_scope.end())
    {
        return false;
    }

    int current_level = scopes.size() - 1;
    current_scope[name] = Symbol{name, type_token, cat, current_level};

    return true;
}

Symbol *SymbolTable::lookup(const std::string &name)
{

    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        auto found = it->find(name);
        if (found != it->end())
        {
            return &(found->second);
        }
    }
    return nullptr;
}

Symbol *SymbolTable::lookupCurrentScope(const std::string &name)
{
    auto &current_scope = scopes.back();
    auto found = current_scope.find(name);
    if (found != current_scope.end())
    {
        return &(found->second);
    }
    return nullptr;
}

std::string SymbolTable::categoryToString(SymbolCategory cat)
{
    switch (cat)
    {
    case SymbolCategory::VAR:
        return "Variável";
    case SymbolCategory::FUNC:
        return "Função";
    case SymbolCategory::PARAM:
        return "Parâmetro";
    default:
        return "Desconhecido";
    }
}

void SymbolTable::printTable() const
{
    std::cout << "\n================ TABELA DE SÍMBOLOS ================\n";
    std::cout << std::left
              << std::setw(20) << "NOME"
              << std::setw(15) << "CATEGORIA"
              << std::setw(15) << "TOKEN_TIPO"
              << std::setw(10) << "NÍVEL_ESCOPO" << "\n";
    std::cout << "----------------------------------------------------\n";

    for (size_t i = 0; i < scopes.size(); ++i)
    {
        for (const auto &pair : scopes[i])
        {
            const Symbol &s = pair.second;
            std::cout << std::left
                      << std::setw(20) << s.name
                      << std::setw(15) << categoryToString(s.category)
                      << std::setw(15) << s.type_token
                      << std::setw(10) << s.scope_level << "\n";
        }
    }
    std::cout << "====================================================\n\n";
}
