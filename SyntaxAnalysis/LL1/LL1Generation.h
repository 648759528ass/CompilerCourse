//
// Created by jhas on 2022/6/13.
//

#ifndef SYNTAXANALYSIS_LL1GENERATION_H
#define SYNTAXANALYSIS_LL1GENERATION_H

#include "../Grammar/Grammar.h"

class LL1Generation {
public:
    Grammar& G;
    LL1Generation(Grammar& G);
    std::unordered_map<char,std::unordered_map<char,std::vector<char>>> analysisTable;
    bool analyse(std::string input,std::vector<std::tuple<std::string,std::string,std::string,std::string>>& analyseProcess);
    std::unordered_map<char,std::unordered_map<char,std::vector<char>>> getAnalysisTable();
};


#endif //SYNTAXANALYSIS_LL1GENERATION_H
