//
// Created by jhas on 2022/6/13.
//

#include "LL1Generation.h"
#include "../ErrorHandle/errorHandle.h"
#include <deque>
LL1Generation::LL1Generation(Grammar& G):G(G){
    std::unordered_map<char,std::unordered_set<symbol>> First = G.allFirst();
    std::unordered_map<char,std::unordered_set<symbol>> Follow = G.allFollow();
    auto productions = G.getProductions();
    for(auto nonTerminal:G.getNonTerminalSet()){
        for(std::vector<char> production:productions[nonTerminal]){
            bool epsilon = true;
            std::unordered_set<char> st;
            if(production.size() > 1 || production[0] != '\0'){
                for(auto sub:production){
                    for(auto c:First[sub]){
                        if(c!='\0')
                            st.insert(c);
                    }
                    if(First[sub].count('\0') == 0){
                        epsilon = false;
                        break;
                    }
                }
            }
            if(epsilon){
                for(auto c:Follow[nonTerminal]){
                    if(c!='\0')
                        st.insert(c);
                }
            }
            for(auto c:st){
                if(analysisTable[nonTerminal].count(c)&&analysisTable[nonTerminal][c]!=production) error("there is not LL(1) for this grammar");
                analysisTable[nonTerminal][c] = production;
            }
        }
    }
}
bool LL1Generation::analyse(std::string input,std::vector<std::tuple<std::string,std::string,std::string,std::string>>& analyseProcess){
    std::unordered_map<char,std::unordered_set<symbol>> First = G.allFirst();
    std::unordered_map<char,std::unordered_set<symbol>> Follow = G.allFollow();
    std::unordered_set<symbol> nonTerminalSet = G.getNonTerminalSet();
    std::deque<symbol> st;
    st.push_front(G.getStart());
    int i = 0;
    int step = 1;
    std::string finish = "";
    analyseProcess.emplace_back(finish,std::string(st.begin(),st.end())+"$",std::string(input.begin()+i,input.end())+"$","");
    while(!st.empty()){
        std::string operation;
        symbol inputFront = (i == input.size() ? '$' :input[i]);
        if(nonTerminalSet.count(st.front())){
            if(analysisTable[st.front()].count(inputFront) == 0){
                return false;
            }else{
                auto front = st.front();
                st.pop_front();
                if(analysisTable[front][inputFront].size() >1||analysisTable[front][inputFront][0] != '\0'){
                    for(auto r = analysisTable[front][inputFront].rbegin();r!=analysisTable[front][inputFront].rend();r++){
                        st.push_front(*r);
                    }
                }
                auto& tmp = analysisTable[st.front()][inputFront];
                operation = "output " + std::string(1,front) +"->"
                        +((tmp.size() == 1&&tmp[0] == '\0')?"ε":std::string(tmp.begin(),tmp.end()));
            }

        }else{
            operation = "match " + std::string(1,input[i++]);
            finish+=st.front();
            st.pop_front();
        }
        analyseProcess.emplace_back(finish,std::string(st.begin(),st.end())+"$",std::string(input.begin()+i,input.end())+"$",operation);
    }
    return true;
}
std::unordered_map<char,std::unordered_map<char,std::vector<char>>> LL1Generation::getAnalysisTable(){
    return analysisTable;
}