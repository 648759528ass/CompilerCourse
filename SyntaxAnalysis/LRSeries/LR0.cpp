//
// Created by jhas on 2022/6/14.
//
#include <sstream>
#include <iostream>
#include "LR0.h"
#include "../ErrorHandle/errorHandle.h"

void printItem(std::set<std::tuple<symbol, std::vector<symbol>, int>> I) {
    for (auto c:I) {
        auto tmp = std::get<1>(c);
        tmp.insert(tmp.begin() + std::get<2>(c), '.');
        std::cout << std::get<0>(c) << "->" << std::string(tmp.begin(), tmp.end()) << "\n";
    }
}

std::set<std::tuple<symbol, std::vector<symbol>, int>>
LR0::closure(std::set<std::tuple<symbol, std::vector<symbol>, int>> I) {
    auto productions = G.getProductions();
    auto nonTerminalSet = G.getNonTerminalSet();
    std::vector<std::tuple<symbol, std::vector<symbol>, int>> tmp;
    do {
        tmp.clear();
        for (auto&[s, w, p]:I) {
            if(w.size() == 1&&w[0] == '\0') continue;
            if (p == w.size()) continue;
            if (nonTerminalSet.count(w[p])) {
                for (auto &v:productions[w[p]]) {
                    auto newOne = std::make_tuple(w[p], v, 0);
                    if (I.count(newOne) == 0) tmp.push_back(newOne);
                }
            }
        }
        if (tmp.empty()) break;
        for (auto &item:tmp) {
            I.insert(item);
        }
    } while (true);
    return I;
}

std::set<std::tuple<symbol, std::vector<symbol>, int>>
LR0::GOTO(const std::set<std::tuple<symbol, std::vector<symbol>, int>> &I, symbol X) {
    std::set<std::tuple<symbol, std::vector<symbol>, int>> next;
    for (auto&[s, w, p]:I) {
        if (p == w.size() || w[p] != X) continue;
        else next.insert({s, w, p + 1});
    }
    return closure(next);
}

void LR0::generateCollection() {
    auto s1 = G.getNonTerminalSet();
    s1.erase(G.Start);
    auto s2 = G.getTerminalSet();
    int n = collection.size();
    for (int i = 0; i < n; i++) {
        for (auto s:s1) {
            auto tmp = GOTO(collection[i], s);
            if (!tmp.empty() && find(tmp) == -1) {
                collection.push_back(tmp);
            }
        }
        for (auto s:s2) {
            auto tmp = GOTO(collection[i], s);
            if (!tmp.empty() && find(tmp) == -1) {
                collection.push_back(tmp);
            }
        }
        n = collection.size();
    }
}

void LR0::generateTable() {
    auto terminalSet = G.getTerminalSet();
    auto nonTerminalSet = G.getNonTerminalSet();
    terminalSet.insert('$');
    for (int i = 0; i < collection.size(); i++) {
        auto thisStatus = collection[i];
        analysisTable.emplace_back();
        std::vector<std::tuple<symbol, std::vector<symbol>, int>> shift, reduce;
        for (auto c:thisStatus) {
            if (std::get<1>(c).size() == std::get<2>(c)) {
                reduce.push_back(c);
            } else {
                if(std::get<1>(c).size() == 1&&std::get<1>(c)[0] == '\0') continue;
                shift.push_back(c);
            }
        }
        if (reduce.size() > 1) error("含有规约-规约冲突");
        if (reduce.size() == 1) {
            if (std::get<0>(reduce[0]) == productionList[0].first &&
                std::get<1>(reduce[0]) == productionList[0].second) {

                analysisTable[i].first['$'] = {'a', 0};
            } else {
                int reduceIdx = -1;
                for (int j = 1; j < productionList.size(); j++) {
                    if (std::get<0>(reduce[0]) == productionList[j].first &&
                        std::get<1>(reduce[0]) == productionList[j].second) {
                        reduceIdx = j;
                        break;
                    }
                }
                if (reduceIdx == -1) error("no reduce");
                for (auto s:terminalSet) {
                    if(analysisTable[i].first.count(s)!=0&&analysisTable[i].first[s] != std::make_pair('r', reduceIdx)){
                        error("action表出现冲突");
                    }
                    analysisTable[i].first[s] = {'r', reduceIdx};
                }
            }
        }
        if (!shift.empty()) {
            for (auto sh:shift) {
                auto tmpc = std::get<1>(sh)[std::get<2>(sh)];
                auto tmp = GOTO(thisStatus, tmpc);
                int idx = find(tmp);
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

int LR0::find(const std::set<std::tuple<symbol, std::vector<symbol>, int>> &I) {
    for (int i = 0; i < collection.size(); i++) {
        if (collection[i] == I) return i;
    }
    return -1;
}

LR0::LR0(Grammar &G) : G(G) {
    auto start = G.changeStart();
    auto productions = G.getProductions();
    std::set<std::tuple<symbol, std::vector<symbol>, int>> s;
    s.insert({start, productions[start][0], 0});
    s = closure(s);
    //printItem(s);
    collection.push_back(s);
    generateCollection();
    auto nonTerminals = G.getNonTerminalSet();
    nonTerminals.erase(start);
    productionList.emplace_back(start, productions[start][0]);
    for (auto &nonTerminal:nonTerminals) {
        for (auto &v:productions[nonTerminal]) {
            productionList.emplace_back(nonTerminal, v);
        }
    }
}

AnalysisTable LR0::getAnalysisTable() {
    if (analysisTable.empty()) generateTable();
    return analysisTable;
}

ProductionList LR0::getProductionList() {
    return productionList;
}
std::string LR0::generateGraphvizStr(){
    if(analysisTable.empty()) generateTable();
    std::stringstream out;
    out<<"digraph G {\nrankdir=LR\n";
    for(int i = 0;i<collection.size();i++){
        out<<i <<" [shape = record, label = \"";
        for (auto c:collection[i]) {
            auto tmp = std::get<1>(c);
            tmp.insert(tmp.begin() + std::get<2>(c), '.');
            out << std::get<0>(c) << "-&gt;" << std::string(tmp.begin(), tmp.end()) << "|";
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