//
// Created by jhas on 2022/6/15.
//
#include <sstream>
#include "LR1.h"
#include "../ErrorHandle/errorHandle.h"
void LR1::generateCollection() {
    auto s1 = G.getNonTerminalSet();
    auto s2 = G.getTerminalSet();
    s2.erase('\0');
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

LR1::LR1(Grammar &G) : G(G) {
    auto start = G.changeStart();
    auto productions = G.getProductions();
    ItemSet s;
    s.insert({{start, productions[start][0], 0},
              {'$'}});
    closure(s);
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

int LR1::find(const ItemSet &I) {
    for (int i = 0; i < collection.size(); i++) {
        if (collection[i] == I) return i;
    }
    return -1;
}

std::vector<symbol> LR1::First(std::vector<symbol> suffix) {
    std::vector<symbol> ret;
    auto exist = [&ret](symbol s) -> bool {
        for (auto &c:ret) {
            if (s == c) return true;
        }
        return false;
    };
    auto firstSet = G.allFirst();
    for (int i = 0; i < suffix.size(); i++) {
        if(suffix[i] == '$'){
            if(!exist('$')) ret.push_back('$');
            break;
        }
        for (auto c:firstSet[suffix[i]]) {
            if (c != '\0' && !exist(c)) ret.push_back(c);
        }
        if (firstSet[suffix[i]].count('\0') == 0) {
            break;
        }
    }
    return ret;
}

void LR1::closure(ItemSet& I) {
    auto productions = G.getProductions();
    auto nonTerminalSet = G.getNonTerminalSet();
    std::vector<Item> tmp;
    do {
        tmp.clear();
        for (auto&[core, front]:I) {
            auto&[s, w, p] = core;
            if(w.size() == 1&&w[0] == '\0') continue;
            if (p == w.size()) continue;
            std::vector<symbol> suffix;
            if (nonTerminalSet.count(w[p])) {
                for (int i = p + 1; i < w.size(); i++) {
                    suffix.push_back(w[i]);
                }
                suffix.push_back(front);
                auto suffixSet = First(suffix);
                for (auto &v:productions[w[p]]) {
                    for(auto c:suffixSet){
                        auto newOne = std::make_pair(std::make_tuple(w[p], v, 0),c);
                        if (I.count(newOne) == 0) tmp.push_back(newOne);
                    }
                }
            }
        }
        if (tmp.empty()) break;
        for (auto &item:tmp) {
            I.insert(item);
        }
    } while (true);
}

ItemSet LR1::GOTO(const ItemSet &I, symbol X) {
    std::set<Item> next;
    for (auto&[core,front]:I) {
        auto&[s, w, p] = core;
        if (p == w.size() || w[p] != X) continue;
        else next.insert({{s, w, p + 1},front});
    }
    closure(next);
    return next;
}

void LR1::generateTable() {
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
                int idx = find(tmp);
                if (idx == -1){
                    error("cannot find the status");
                }
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


AnalysisTable LR1::getAnalysisTable() {
    if (analysisTable.empty()) generateTable();
    return analysisTable;
}

ProductionList LR1::getProductionList() {
    return productionList;
}
std::string LR1::generateGraphvizStr() {
    if(analysisTable.empty()) generateTable();
    std::stringstream out;
    out<<"digraph G {\nrankdir=LR\n";
    for(int i = 0;i<collection.size();i++){
        out<<i <<" [shape = record, label = \"";
        for (auto [c,front]:collection[i]) {
            auto tmp = std::get<1>(c);
            if(tmp[0] == '\0'){
                tmp[0] = '-';
            }
            tmp.insert(tmp.begin() + std::get<2>(c), '.');
            out << std::get<0>(c) << " =&gt; " << std::string(tmp.begin(), tmp.end()) <<" , '"<<std::string(1,front) <<"' | ";
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