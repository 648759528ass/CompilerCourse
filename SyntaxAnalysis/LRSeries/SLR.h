//
// Created by jhas on 2022/6/14.
//

#ifndef SYNTAXANALYSIS_SLR_H
#define SYNTAXANALYSIS_SLR_H

#include "LR0.h"
class SLR:public LR0 {
protected:
    void generateTable();
public:
    SLR(Grammar& G);
    AnalysisTable getAnalysisTable() override;
    ProductionList getProductionList() override;
    std::string generateGraphvizStr() override;
};


#endif //SYNTAXANALYSIS_SLR_H
