//
// Created by jhas on 2022/6/13.
//

#ifndef SYNTAXANALYSIS_GRAMMAR_H
#define SYNTAXANALYSIS_GRAMMAR_H
#define symbol_type char
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iostream>

#if(symbol_type == char)
    typedef char symbol;
#endif

typedef std::vector<std::vector<symbol>> Production;
class Grammar {
private:
    std::unordered_map<char,std::unordered_set<symbol>> First;
    std::unordered_map<char,std::unordered_set<symbol>> Follow;
    std::unordered_set<symbol> terminalSet;
    std::unordered_set<symbol> nonTerminalSet;
    std::unordered_map<symbol,Production> productions;

    symbol getNewNonTerminal();
    void computeFirstAndFollow();
public:
#if (symbol_type == char)
    Grammar(std::vector<std::string>);
    Grammar(symbol start,std::vector<std::string>);
    symbol Start = '\0';
#endif
    const std::unordered_set<symbol>& getFirst(symbol S);
    const std::unordered_set<symbol>& getFollow(symbol S);
    void cleanLeftRecursion();
    void drawLeftCommonFactor();
    friend std::ostream& operator<<(std::ostream&,Grammar& g);
    const std::unordered_map<char,std::unordered_set<symbol>>& allFirst();
    const std::unordered_map<char,std::unordered_set<symbol>>& allFollow();
    const std::unordered_set<symbol>& getTerminalSet();
    const std::unordered_set<symbol>& getNonTerminalSet();
    const std::unordered_map<symbol,Production>& getProductions();
    symbol getStart();
    symbol changeStart();
};


#endif //SYNTAXANALYSIS_GRAMMAR_H
