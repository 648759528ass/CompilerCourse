//
// Created by jhas on 2022/6/14.
//

#ifndef SYNTAXANALYSIS_LRANALYSE_H
#define SYNTAXANALYSIS_LRANALYSE_H

#include "LRBase.h"
#include <iostream>
class LRAnalyse {
private:
    AnalysisTable analysisTable;
    ProductionList productionList;
public:
    LRAnalyse(LRBase*);
    bool analyse(const std::string& input,std::vector<std::tuple<std::string,std::string,std::string,std::string>>& analyseProcess);
};


#endif //SYNTAXANALYSIS_LRANALYSE_H
