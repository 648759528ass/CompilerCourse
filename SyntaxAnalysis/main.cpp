#include <iostream>
#include <fstream>
#include "Grammar/Grammar.h"
#include "LL1/LL1Generation.h"
#include "LRSeries/LR0.h"
#include "LRSeries/SLR.h"
#include "LRSeries/LR1.h"
#include "LRSeries/LALR.h"
#include "LRSeries/LRAnalyse.h"
#include "ErrorHandle/errorHandle.h"

void print_LL1AnalyseTable(Grammar &G, std::ostream *outPtr,
                           std::unordered_map<char, std::unordered_map<char, std::vector<char>>> analyseTable) {
    auto &out = (*outPtr);
    auto nonTs = G.getNonTerminalSet();
    auto T = G.getTerminalSet();
    T.insert('$');
    out << "| 非终结符 | ";
    for (auto t:T) {
        out << t << " | ";
    }
    out << "\n";
    out << "| ----- | ";
    for (auto t:T) {
        out << "----- | ";
    }
    out << "\n";
    for(auto nonT:nonTs){
        out<<"| " + std::string(1,nonT) + " | ";
        for(auto t:T){
            if(analyseTable[nonT].count(t)!=0)
                out<<std::string(1,nonT) + "->" << std::string(analyseTable[nonT][t].begin(),analyseTable[nonT][t].end());
            out<<" | ";
        }
        out<<"\n";
    }
}

void printLL1AnalyseProcess(std::ostream *outPtr,
                            std::vector<std::tuple<std::string, std::string, std::string, std::string>> analyseProcess) {
    auto &out = (*outPtr);
    out << "| 步骤 | 已匹配 | 栈 | 输入 | 动作 |\n";
    out << "| ----- | ----- | ----- | ----- | ----- |\n";
    for (int i = 1; i <= analyseProcess.size(); i++) {
        out << "| " << i << " | " << std::get<0>(analyseProcess[i - 1]) << " | " << std::get<1>(analyseProcess[i - 1])
            << " | " << std::get<2>(analyseProcess[i - 1]) << " | " << std::get<3>(analyseProcess[i - 1]) << " |\n";
    }
    out<<"\n";

}

void print_LRAnalyseTable(Grammar &G, std::ostream *outPtr, AnalysisTable analysisTable) {
    auto &out = (*outPtr);
    auto nonT = G.getNonTerminalSet();
    nonT.erase(G.Start);
    auto T = G.getTerminalSet();
    T.insert('$');
    out << "| status | ";
    for (auto t:T) {
        out << t << " | ";
    }
    for (auto t: nonT) {
        out << t << " | ";
    }
    out << "\n";
    out << "| ----- | ";
    for (auto t:T) {
        out << "----- | ";
    }
    for (auto t: nonT) {
        out << "----- | ";
    }
    out << "\n";
    for (size_t i = 0; i < analysisTable.size(); i++) {
        out << "| " << i << " | ";
        for (auto t:T) {
            if (analysisTable[i].first.count(t) != 0) {
                out << analysisTable[i].first[t].first << analysisTable[i].first[t].second << " | ";
            } else {
                out << "  | ";
            }
        }
        for (auto t:nonT) {
            if (analysisTable[i].second.count(t) != 0) {
                out << analysisTable[i].second[t] << " | ";
            } else {
                out << "  | ";
            }
        }
        out << "\n";
    }
    out << "\n";
}
void print_printLR1AnalyseProcess(std::ostream *outPtr,std::vector<std::tuple<std::string,std::string,std::string,std::string>>& analyseProcess){
    auto &out = (*outPtr);
    out << "| 步骤 | 状态栈 | 符号栈 | 输入 | 动作 |\n";
    out << "| ----- | ----- | ----- | ----- | ----- |\n";
    for (int i = 1; i <= analyseProcess.size(); i++) {
        out << "| " << i << " | " << std::get<0>(analyseProcess[i - 1]) << " | " << std::get<1>(analyseProcess[i - 1])
            << " | " << std::get<2>(analyseProcess[i - 1]) << " | " << std::get<3>(analyseProcess[i - 1]) << " |\n";
    }
    out<<"\n";
}
void printProduction(std::ostream *outPtr,Grammar& G){
    auto ps = G.getProductions();
    auto &out = (*outPtr);
    out << "| 生成项 |\n";
    out << "| ----- |  \n";
    for(auto&[s,v]:ps){
        for(auto v1:v){
            out<<"| "<<std::string(1,s) + "->" + std::string(v1.begin(),v1.end())<<" | \n";
        }
    }
    out<<"\n";
}
void printLRProduction(std::ostream *outPtr,ProductionList productionList){
    auto &out = (*outPtr);
    out << "| id | 生成项 |\n";
    out << "| ----- | ----- | \n";
    for (int i = 0; i < productionList.size(); i++) {
        out << "| " << i << " | " << std::string(1,productionList[i].first) + "->" + std::string(productionList[i].second.begin(),productionList[i].second.end())<<" |\n";
    }
    out<<"\n";
}
int main() {
    auto &errorInst = Error::getError();
    std::cout<<"请输入文法条数:";
    int n;
    std::cin>>n;
    std::cout<<"请输入起始符号:";
    symbol S;
    std::cin>>S;
    getchar();
    std::cout<<"请输入生成式:\n";
    std::vector<std::string> strs;
    for(int i = 0;i<n;i++){
        std::string tmp;
        std::getline(std::cin,tmp,'\n');
        strs.push_back(tmp);
    }
    while(1){
        Grammar G(S,strs);
        if(errorInst.getCount() > 0){
            std::cout<<"语法出错";
            return 0;
        }
        std::cout<<"请选择语法分析方法：\n1 : LL(1)分析 \n2:LR(0)分析\n3:SLR(1)分析\n4:LR(1)分析\n5:LALR分析\n";
        int ops;
        std::cin>>ops;
        if(ops == 1){
            G.cleanLeftRecursion();
            G.drawLeftCommonFactor();
            LL1Generation LL1G(G);
            if(errorInst.getCount() > 0){
                std::cout<<"该文法不支持LL(1)";
                continue;
            }
            printProduction(&std::cout,G);
            print_LL1AnalyseTable(G,&std::cout,LL1G.getAnalysisTable());
            std::vector<std::tuple<std::string,std::string,std::string,std::string>> analyseProcess;
            while(1){
                analyseProcess.clear();
                std::string tmp;
                std::cin>>tmp;
                if(LL1G.analyse(tmp,analyseProcess)){
                    std::cout<<"分析成功，语句属于该语法\n";
                    print_printLR1AnalyseProcess(&std::cout,analyseProcess);
                }else{
                    std::cout<<"分析失败\n";
                }
            }
        }else{
            LRBase* lrBase = nullptr;
            switch (ops){
                case 2:{
                    lrBase = new LR0(G);
                    break;
                }
                case 3:{
                    lrBase = new SLR(G);
                    break;
                }
                case 4:{
                    lrBase = new LR1(G);
                    break;
                }
                case 5:{
                    lrBase = new LALR(G);
                    break;
                }
                default:{
                    std::cout<<"选择有误\n";
                    continue;
                }
            }
            auto analyseTable = lrBase->getAnalysisTable();
            if(errorInst.getCount() > 0){
                errorInst.reset();
                std::cout<<"该文法不属于该语法，请尝试使用其它语法\n";
//                continue;
            }

            auto productionList = lrBase->getProductionList();
            printLRProduction(&std::cout,productionList);
            print_LRAnalyseTable(G,&std::cout,analyseTable);
            std::vector<std::tuple<std::string,std::string,std::string,std::string>> analyseProcess;
            std::ofstream DFA("DFA.dot");
            DFA<<lrBase->generateGraphvizStr();
            system("dot -Tpng DFA.dot -o DFA.png");
            LRAnalyse lrAnalyse(lrBase);
            while(1){
                analyseProcess.clear();
                std::string tmp;
                std::cin>>tmp;
                if(lrAnalyse.analyse(tmp,analyseProcess)){
                    std::cout<<"分析成功，语句属于该语法\n";
                    print_printLR1AnalyseProcess(&std::cout,analyseProcess);
                }else{
                    std::cout<<"分析失败\n";
                }
            }
        }
    }


}
