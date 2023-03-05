//
// Created by jhas on 2022/6/13.
//
#include <iostream>
#include "Grammar.h"
using namespace std;
void print(string type,std::unordered_map<char,std::unordered_set<symbol>> st){
    for(auto&[k,v]:st){
        cout<<type<<"("<< k <<") = { ";
        for(auto val:v){
            cout<<(val == '\0'? "ε" : std::string(1,val))<<" ";
        }
        cout<<"}\n";
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
//    G.cleanLeftRecursion();
//    G.drawLeftCommonFactor();
    auto&& Firsts = G.allFirst();
    auto&& Follows = G.allFollow();
    cout << "First:";
    print("First",Firsts);
    cout << "Follow:";
    print("Follow",Follows);
    cout<<G;
    getchar();
}
