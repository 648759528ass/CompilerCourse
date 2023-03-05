//
// Created by jhas on 2022/6/13.
//
#include <iostream>
#include "LL1Generation.h"
using namespace std;
void printLL1AnalysisTable(std::unordered_map<char,std::unordered_map<char,std::vector<char>>> analysisTable){
    for(auto &[k1,v1]:analysisTable){
        cout<<k1<<":\n";
        for(auto &[k2,v2]:v1){
            cout<<"     "<<k2<<": "<<((v2.size() == 1&&v2[0] == '\0')?"ε": string(v2.begin(),v2.end()))<<"\n";
        }
        cout<<"\n";
    }
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
int main(){
    std::vector<std::string> productionStrs;
    int n;
    cin>>n;
    getchar();
    while(n--){
        string s;
        std::getline(cin,s,'\n');
        productionStrs.push_back(s);
    }
    Grammar G(productionStrs);
    G.cleanLeftRecursion();
    G.drawLeftCommonFactor();
    printProduction(&std::cout,G);
}
