//
// Created by jhas on 2022/6/15.
//

#ifndef SYNTAXANALYSIS_LALR_H
#define SYNTAXANALYSIS_LALR_H
#include <vector>
#include <set>
#include <tuple>
#include "LRBase.h"
#include "LR1.h"
typedef std::tuple<symbol ,std::vector<symbol>, int> Core;
typedef std::tuple<Core,symbol> Item;
typedef std::set<Item> ItemSet;
class LALR: public LR1 {

public:
    LALR(Grammar& G);
    void generateTable();
    AnalysisTable getAnalysisTable() override;
    ProductionList getProductionList() override;
    std::string generateGraphvizStr() override ;

};


#endif //SYNTAXANALYSIS_LALR_H
