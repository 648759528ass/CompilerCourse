//
// Created by jhas on 2022/6/14.
//

#include "LRAnalyse.h"
#include <stack>

LRAnalyse::LRAnalyse(LRBase *base) {
    productionList = base->getProductionList();
    analysisTable = base->getAnalysisTable();
}

void collectInfo(std::deque<int> &statusStack, std::deque<symbol> &symbolStack, int pos, std::string input,
                 std::string operation,
                 std::vector<std::tuple<std::string, std::string, std::string, std::string>> &analyseProcess) {
    std::string status, symbol, left;
    for (int i = 0; i < statusStack.size(); i++) {
        status += std::to_string(statusStack[i])+" ";
    }
    for (int i = 0; i < symbolStack.size(); i++) {
        symbol += std::to_string(symbolStack[i])+" ";
    }
    for (int i = pos; i < input.size(); i++) {
        left += input[i];
    }
    analyseProcess.emplace_back(status, symbol, left, operation);
}

bool LRAnalyse::analyse(const std::string &input,
                        std::vector<std::tuple<std::string, std::string, std::string, std::string>> &analyseProcess) {
    std::deque<int> statusStack;
    std::deque<symbol> symbolStack;
    statusStack.push_back(0);
    int i = 0;
    while (1) {
        int s = statusStack.back();
        char inputFront = i != input.size() ? input[i] : '$';
        auto &Action = analysisTable[s].first;
        auto &Goto = analysisTable[s].second;
        if (Action.count(inputFront) && Action[inputFront].first == 's') {
            statusStack.push_back(Action[input[i]].second);
            symbolStack.push_back(input[i++]);
            collectInfo(statusStack, symbolStack, i, input, "移入" + inputFront, analyseProcess);
        } else if (Action.count(inputFront) && Action[inputFront].first == 'r') {
            auto production = productionList[Action[inputFront].second];
            for (int j = 0; j < production.second.size(); j++) {
                if (j != 0) statusStack.pop_back();
                symbolStack.pop_back();
            }
            statusStack.pop_back();
            s = statusStack.back();
            auto &Goto = analysisTable[s].second;
            statusStack.push_back(Goto[production.first]);
            symbolStack.push_back(production.first);
            collectInfo(statusStack, symbolStack, i, input, "归约" + std::string(1, production.first) + "->" +
                                                            std::string(production.second.begin(),
                                                                        production.second.end()), analyseProcess);
        } else if (Action.count(inputFront) && Action[inputFront].first == 'a') {
            collectInfo(statusStack, symbolStack, i, input, "acc", analyseProcess);
            return true;
        } else {
            return false;
        }
    }
}