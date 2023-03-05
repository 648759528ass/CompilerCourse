//
// Created by jhas on 2022/6/14.
//

#ifndef SYNTAXANALYSIS_LR0_H
#define SYNTAXANALYSIS_LR0_H

#include "LRBase.h"
#include <vector>
#include <set>
#include <tuple>
class LR0: public LRBase {
protected:
    AnalysisTable analysisTable;
    std::vector<std::set<std::tuple<symbol ,std::vector<symbol>, int>>> collection;
    ProductionList productionList;
    Grammar& G;
    void generateCollection();
    void generateTable();
    std::set<std::tuple<symbol ,std::vector<symbol>, int>> closure(std::set<std::tuple<symbol ,std::vector<symbol>, int>> I);
    std::set<std::tuple<symbol ,std::vector<symbol>, int>> GOTO(const std::set<std::tuple<symbol ,std::vector<symbol>, int>>& I,symbol X);
    int find(const std::set<std::tuple<symbol ,std::vector<symbol>, int>>& I);

public:
    LR0(Grammar& G);
    virtual AnalysisTable getAnalysisTable() override;
    virtual ProductionList getProductionList() override;
    std::string generateGraphvizStr();
};


#endif //SYNTAXANALYSIS_LR0_H
