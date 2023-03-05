//
// Created by jhas on 2022/6/15.
//

#ifndef SYNTAXANALYSIS_LR1_H
#define SYNTAXANALYSIS_LR1_H
#include <vector>
#include <set>
#include <tuple>
#include "LRBase.h"
typedef std::tuple<symbol ,std::vector<symbol>, int> Core;
typedef std::tuple<Core,symbol> Item;
typedef std::set<Item> ItemSet;
class LR1:public LRBase {
protected:
    AnalysisTable analysisTable;
    std::vector<ItemSet> collection;
    ProductionList productionList;
    Grammar& G;
    void generateCollection();
    void generateTable();
    void closure(ItemSet& I);
    ItemSet GOTO(const ItemSet& I,symbol X);
    std::vector<symbol> First(std::vector<symbol> suffix);
    int find(const ItemSet& I);
public:
    LR1(Grammar&);
    AnalysisTable getAnalysisTable() override;
    ProductionList getProductionList() override;
    std::string generateGraphvizStr() override;
};


#endif //SYNTAXANALYSIS_LR1_H
