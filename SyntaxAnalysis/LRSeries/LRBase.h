//
// Created by jhas on 2022/6/14.
//

#ifndef SYNTAXANALYSIS_LRBASE_H
#define SYNTAXANALYSIS_LRBASE_H

#include <unordered_map>
#include <utility>
#include "../Grammar/Grammar.h"
typedef std::vector<std::pair<std::unordered_map<symbol,std::pair<char,int>>,std::unordered_map<symbol,int>>> AnalysisTable;
typedef std::vector<std::pair<symbol,std::vector<symbol>>> ProductionList;
class LRBase {
public:
    virtual AnalysisTable getAnalysisTable() = 0;
    virtual ProductionList getProductionList() = 0;
    virtual std::string generateGraphvizStr() = 0;
};


#endif //SYNTAXANALYSIS_LRBASE_H
