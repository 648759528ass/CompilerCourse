//
// Created by jhas on 2022/6/15.
//
#include <sstream>
#include "LALR.h"
#include "../ErrorHandle/errorHandle.h"
std::set<Core> buildCoreSet(ItemSet IS){
    std::set<Core> coreSet;
    for(auto&I:IS){
        auto& tmp = std::get<0>(I);
        if(coreSet.count(tmp) == 0){
            coreSet.insert(tmp);
        }
    }
    return coreSet;
}
LALR::LALR(Grammar& G):LR1(G){
    std::vector<ItemSet> newItemSet;
    auto findCoreSet = [&newItemSet](const std::set<Core>& st)->int {
        for(int i = 0;i<newItemSet.size();i++){
            if(st == buildCoreSet(newItemSet[i])){
                return i;
            }
        }
        return -1;
    };
    for(auto& IS:collection){
        int idx = findCoreSet(buildCoreSet(IS));
        if(idx!=-1){
            for(auto& s:IS){
                newItemSet[idx].insert(s);
            }
        }else{
            newItemSet.push_back(IS);
        }
    }
    collection = newItemSet;
}
void LALR::generateTable(){
    auto findCoreSet = [this](const std::set<Core>& st)->int {
        for(int i = 0;i<collection.size();i++){
            if(st == buildCoreSet(collection[i])){
                return i;
            }
        }
        return -1;
    };
    auto terminalSet = G.getTerminalSet();
    auto nonTerminalSet = G.getNonTerminalSet();
    auto productions = G.getProductions();
    terminalSet.insert('$');
    for (int i = 0; i < collection.size(); i++) {
        auto thisStatus = collection[i];
        analysisTable.emplace_back();
        std::vector<std::tuple<symbol, std::vector<symbol>, int>> shift;
        std::vector<Item>reduce;
        for (auto item:thisStatus) {
            auto&[c,f] = item;
            if (std::get<1>(c).size() == std::get<2>(c)) {
                reduce.push_back({c,f});
            } else {
                if(std::get<1>(c).size() == 1&&std::get<1>(c)[0] == '\0') continue;
                shift.push_back(c);
            }
        }
        for(auto& [core,front]:reduce){
            auto&[s,w,p] = core;
            if(s == G.getStart()&&w == productionList[0].second){
                analysisTable[i].first['$'] = {'a', 0};
            }else{
                int reduceIdx = -1;
                for (int j = 1; j < productionList.size(); j++) {
                    if (s == productionList[j].first &&
                        w == productionList[j].second) {
                        reduceIdx = j;
                        break;
                    }
                }
                if (reduceIdx == -1) error("no reduce");
                if(analysisTable[i].first.count(front)!=0&&analysisTable[i].first[front] != std::make_pair('r', reduceIdx)){
                    error("action表出现冲突");
                }
                analysisTable[i].first[front] = {'r', reduceIdx};
            }
        }
        if (!shift.empty()) {
            for (auto sh:shift) {
                auto tmpc = std::get<1>(sh)[std::get<2>(sh)];
                auto tmp = GOTO(thisStatus, tmpc);
                int idx = findCoreSet(buildCoreSet(tmp));
                if (idx == -1) error("cannot find the status");
                if (nonTerminalSet.count(tmpc)) {
                    if(analysisTable[i].second.count(tmpc)!=0&&analysisTable[i].second[tmpc]!=idx){
                        error("goto表出现冲突");
                    }
                    analysisTable[i].second[tmpc] = idx;
                } else {
                    if(analysisTable[i].first.count(tmpc)!=0&&analysisTable[i].first[tmpc] != std::make_pair('s', idx)){
                        error("action表出现冲突");
                    }
                    analysisTable[i].first[tmpc] = {'s', idx};
                }
            }
        }
    }
}

AnalysisTable LALR::getAnalysisTable() {
    if(analysisTable.empty()) LALR::generateTable();
    return analysisTable;
}

ProductionList LALR::getProductionList() {
    return productionList;
}

std::string LALR::generateGraphvizStr() {
    std::stringstream out;
    out<<"digraph G {\nrankdir=LR\n";
    for(int i = 0;i<collection.size();i++){
        out<<i <<" [shape = record, label = \"";
        for (auto [c,front]:collection[i]) {
            auto tmp = std::get<1>(c);
            tmp.insert(tmp.begin() + std::get<2>(c), '.');
            out << std::get<0>(c) << "-&gt;" << std::string(tmp.begin(), tmp.end()) <<","<<std::string(1,front) <<"|";
        }
        out<<i<<"\"];\n";
    }
    for(int i = 0;i<analysisTable.size();i++){
        auto stat = analysisTable[i];
        for(auto [p,w]:stat.first){
            if(w.first == 's'){
                out << i <<" -> " <<w.second << "[label=\""<<std::string(1,p)<<"\"];\n";
            }
        }
        for(auto [p,w]:stat.second){
            out << i <<" -> " <<w << "[label=\""<<std::string(1,p)<<"\"];\n";
        }
    }
    out<<"}";
    return out.str();
}
