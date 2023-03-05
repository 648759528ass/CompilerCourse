//
// Created by jhas on 2022/6/14.
//
#include <fstream>
#include <iostream>
#include "LR0.h"
#include "SLR.h"
#include "LR1.h"
#include "LALR.h"
#include "LRAnalyse.h"
using namespace std;
void printAnalysisTable(Grammar& G,AnalysisTable tb){
    auto nonT = G.getNonTerminalSet();
    nonT.erase(G.Start);
    auto T = G.getTerminalSet();
    T.insert('$');
    cout<< "| status | ";
    for(auto t:T){
        cout<<t<<" | ";
    }
    for(auto t: nonT){
        cout<<t<<" | ";
    }
    cout<<"\n";
    cout<<"| ----- | ";
    for(auto t:T){
        cout<<"----- | ";
    }
    for(auto t: nonT){
        cout<<"----- | ";
    }
    cout<<"\n";
    for(size_t i = 0;i<tb.size();i++){
        cout<<"| "<<i<<" | ";
        for(auto t:T){
            if(tb[i].first.count(t)!=0){
                cout<<tb[i].first[t].first<<tb[i].first[t].second<<" | ";
            }else{
                cout<<"  | ";
            }
        }
        for(auto t:nonT){
            if(tb[i].second.count(t)!=0){
                cout<< tb[i].second[t]<<" | ";
            }else{
                cout<<"  | ";
            }
        }
        cout<<"\n";
    }
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
    LRBase* lr = new LR1(G);
    printAnalysisTable(G,lr->getAnalysisTable());
//    std::string input;
//    cin>>input;
//    LRAnalyse LRA(lr);
//    bool jg = LRA.analyse(input);
//    if(jg)
//    cout<<"ok";
}