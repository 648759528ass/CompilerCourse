//
// Created by jhas on 2022/6/3.
//

#ifndef SYNTAXANALYSIS_BASESYMBOL_H
#define SYNTAXANALYSIS_BASESYMBOL_H

#include <unordered_set>
#include <vector>
class BaseSymbol {
protected:
    BaseSymbol(){
    }
public:
    virtual std::unordered_set<char> First() = 0;
    virtual std::unordered_set<char> Follow() = 0;

};


#endif //SYNTAXANALYSIS_BASESYMBOL_H
