//
// Created by jhas on 2022/6/14.
//
#include <sstream>
#include "SLR.h"
#include "../ErrorHandle/errorHandle.h"
void SLR::generateTable(){
    auto terminalSet = G.getTerminalSet();
    auto Follow = G.allFollow();
    auto nonTerminalSet = G.getNonTerminalSet();
    terminalSet.insert('$');
    for(int i = 0;i<collection.size();i++){
        auto thisStatus = collection[i];
        analysisTable.emplace_back();
        std::vector<std::tuple<symbol ,std::vector<symbol>, int>> shift,reduce;
        for(auto c:thisStatus){
            if(std::get<1>(c).size() == std::get<2>(c)){
                reduce.push_back(c);
            }else{
                if(std::get<1>(c).size() == 1&&std::get<1>(c)[0] == '\0') continue;
                shift.push_back(c);
            }
        }
        if(reduce.size()>1) error("含有规约-规约冲突");
        if(reduce.size() == 1){
            if(std::get<0>(reduce[0]) == productionList[0].first&&std::get<1>(reduce[0]) == productionList[0].second){
                analysisTable[i].first['$'] = {'a',0};
            } else{
                int reduceIdx = -1;
                for(int j = 1;j<productionList.size();j++){
                    if(std::get<0>(reduce[0]) == productionList[j].first&&std::get<1>(reduce[0]) == productionList[j].second){
                        reduceIdx = j;
                        break;
                    }
                }
                if(reduceIdx == -1) error("no reduce");
                for(auto s:Follow[std::get<0>(reduce[0])]){
                    if(analysisTable[i].first.count(s)!=0&&analysisTable[i].first[s] != std::make_pair('r', reduceIdx)){
                        error("action表出现冲突");
                    }
                    analysisTable[i].first[s] = {'r',reduceIdx};
                }
            }
        }
        if(!shift.empty()){
            for(auto sh:shift){
                auto tmpc = std::get<1>(sh)[std::get<2>(sh)];
                auto tmp = GOTO(thisStatus,tmpc);
                int idx = find(tmp);
                if(idx == -1) error("cannot find the status");
                if(nonTerminalSet.count(tmpc)){
                    if(analysisTable[i].second.count(tmpc)!=0&&analysisTable[i].second[tmpc]!=idx){
                        error("goto表出现冲突");
                    }
                    analysisTable[i].second[tmpc] = idx;
                }else{
                    if(analysisTable[i].first.count(tmpc)!=0&&analysisTable[i].first[tmpc] != std::make_pair('s', idx)){
                        error("action表出现冲突");
                    }
                    analysisTable[i].first[tmpc] = {'s',idx};
                }
            }
        }
    }
}
SLR::SLR(Grammar& G):LR0(G){

}
AnalysisTable SLR::getAnalysisTable(){
    if(analysisTable.empty()) generateTable();
    return analysisTable;
}
ProductionList SLR::getProductionList(){
    return productionList;
}


std::string SLR::generateGraphvizStr(){
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