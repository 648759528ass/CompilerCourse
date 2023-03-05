//
// Created by jhas on 2022/6/13.
//

#include "Grammar.h"
#include "../ErrorHandle/errorHandle.h"
#if(symbol_type == char)
symbol Grammar::getNewNonTerminal(){
    for(char ret = 'A';ret<='Z';ret++){
        if(nonTerminalSet.count(ret)) continue;
        nonTerminalSet.insert(ret);
        return ret;
    }
    error("非终结符溢出");
}
Grammar::Grammar(std::vector<std::string> productionStrs){
    for(auto& productionStr:productionStrs){
        nonTerminalSet.insert(productionStr[0]);
    }
    for (auto& productionStr:productionStrs) {
        if(Start == '\0'){
            Start = productionStr[0];
        }
        auto&& thisProduction = productions[productionStr[0]];
        std::vector<char> sub;
        for(int i = 2;i<productionStr.size();i++){
            if(productionStr[i] == '|'){
                thisProduction.push_back(sub);
                sub.clear();
            }else{
                if(productionStr[i] == '-')
                    sub.push_back('\0');
                else{
                    if(productionStr[i]<'A'||productionStr[i]>'Z'){
                        terminalSet.insert(productionStr[i]);
                    }
                    if(!sub.empty()&&sub.back() == '\0') sub.pop_back();
                    sub.push_back(productionStr[i]);
                }
            }
        }
        if(!sub.empty()) thisProduction.push_back(sub);
    }

}
std::vector<char> replaceFirst(std::vector<char> base,std::vector<char> production){
    for(int i = 1;i<production.size();i++){
        if(production[i]!='\0')
            base.push_back(production[i]);
    }
    return base;
}
void Grammar::cleanLeftRecursion(){
    std::vector<char> nonTerminalSet;
    for(auto&[k,_]:productions){
        nonTerminalSet.push_back(k);
    }
    for(int i = 0;i<nonTerminalSet.size();i++){
        auto&& production = productions[nonTerminalSet[i]];
        for(int j = 0;j<i;j++){
            auto&& infer = productions[nonTerminalSet[j]];
            for(int k = 0;k<production.size();k++){
                if(nonTerminalSet[j] == production[k][0]){
                    auto tmp = production[k];
                    production[k] = replaceFirst(infer[0],tmp);
                    for(int l = 1;l<infer.size();l++) production.push_back(replaceFirst(infer[l],tmp));
                }
            }
        }
        std::vector<std::vector<char>> normals,recursions;
        bool existEpslion = 0;
        for(auto &sub :production){
            if(sub[0] == nonTerminalSet[i]) recursions.push_back(sub);
            else if(sub.size() == 1&&sub[0] == '\0'){
                existEpslion = 1;
                continue;
            }
            else normals.push_back(sub);
        }
        if(recursions.empty()) continue;
        if(normals.empty()) error("exist ring");
        symbol newA = getNewNonTerminal();
        for(auto& sub:normals){
            sub.push_back(newA);
        }
        production.clear();
        for(auto& sub:recursions){
            std::vector<char> newOne;
            for(int i = 1;i<sub.size();i++){
                newOne.push_back(sub[i]);
            }
            newOne.push_back(newA);
            productions[newA].push_back(newOne);
        }
        productions[newA].push_back({'\0'});
        production = normals;
        if(existEpslion)
            production.push_back({newA});
    }
}
std::vector<char> getTheLongestCommonFactor(std::vector<char> A,std::vector<char> B){
    std::vector<char> ret;
    for(int i = 0;i<std::min(A.size(),B.size());i++){
        if(A[i] == B[i]){
            ret.push_back(A[i]);
        } else
            break;
    }
    return ret;
}
bool existCommonFactor(std::vector<char> A,std::vector<char> factor){
    if(A.size() < factor.size()) return false;
    for(int i = 0;i<factor.size();i++){
        if(A[i] != factor[i]) return false;
    }
    return true;
}

void Grammar::drawLeftCommonFactor(){
    std::vector<char> nonTerminalSet;
    for(auto&[k,_]:productions){
        nonTerminalSet.push_back(k);
    }
    for(auto& nonTerminal:nonTerminalSet){
        do{
            std::vector<std::vector<char>> newProduction;
            std::vector<char> common;
            for(int i = 0;i<productions[nonTerminal].size();i++){
                for(int j = 0;j<i;j++){
                    auto tmp = getTheLongestCommonFactor(productions[nonTerminal][i],productions[nonTerminal][j]);
                    if(tmp.size()>common.size()){
                        common = tmp;
                    }
                }
            }
            if(common.size() == 0) break;
            symbol newNonTerminal = getNewNonTerminal();
            for(auto& sub:productions[nonTerminal]){
                if(existCommonFactor(sub,common)){
                    std::vector<char> newP;
                    for(int i = common.size();i<sub.size();i++){
                        newP.push_back(sub[i]);
                    }
                    if(newP.empty()) newP.push_back('\0');
                    productions[newNonTerminal].push_back(newP);
                }else{
                    newProduction.push_back(sub);
                }
            }
            common.push_back(newNonTerminal);
            newProduction.push_back(common);
            productions[nonTerminal] = newProduction;
        }while(true);
    }
}
std::ostream& operator<<(std::ostream& out,Grammar& G){
    for(auto item:G.productions){
        for(auto& val:item.second){
            out<<item.first<<" -> ";
            for(auto c:val){
                out<<(c == '\0'? "ε" : std::string(1,c))<<" ";
            }
            out<<"\n";
        }
    }
    return out;
}
void Grammar::computeFirstAndFollow(){
    bool hasNew;
    do{
        hasNew = false;
        for(auto v:nonTerminalSet){
            for(auto& production:productions[v]){
                if(production.size() == 1&&production[0] == '\0'&&First[v].count('\0')==0){
                    First[v].insert('\0');
                    hasNew = true;
                    continue;
                }
                bool eps = true;
                for(auto c:production){
                    if(terminalSet.count(c) && First[c].empty()){
                        First[c].insert(c);
                        hasNew = true;
                    }
                    for(auto& firstItem:First[c]){
                        if(firstItem != '\0'&&First[v].count(firstItem) == 0){
                            First[v].insert(firstItem);
                            hasNew = true;
                        }
                    }
                    if(First[c].count('\0') == 0){
                        eps = false;
                        break;
                    }
                }
                    if(eps&&First[v].count('\0') == 0){
                        First[v].insert('\0');
                        hasNew = true;
                    }

            }
        }
    }while(hasNew);
    Follow[Start].insert('$');
    do{
        hasNew = false;
        for(auto v:nonTerminalSet){
            for(auto& production:productions[v]){
                if(production.size() == 1&&production[0] == '\0'){
                    continue;
                }
                for(int i = 0;i<production.size();i++){
                    bool epsilon = true;
                    for(int j = i+1;j<production.size();j++){
                        if(terminalSet.count(production[j]) && First[production[j]].empty()){
                            First[production[j]].insert(production[j]);
                            hasNew = true;
                        }
                        for(auto c:First[production[j]]){
                            if(c == '\0') continue;
                            if(Follow[production[i]].count(c) == 0){
                                Follow[production[i]].insert(c);
                                hasNew = true;
                            }
                        }
                        if(First[production[j]].count('\0') == 0){
                            epsilon = false;
                            break;
                        }
                    }
                    if(epsilon){
                        for(auto c:Follow[v]){
                            if(Follow[production[i]].count(c) == 0){
                                Follow[production[i]].insert(c);
                                hasNew = true;
                            }
                        }
                    }
                }
            }
        }
    }while (hasNew);
}
const std::unordered_set<symbol>& Grammar::getFirst(symbol S){
    if(First.empty()) computeFirstAndFollow();
    return First[S];
}
const std::unordered_set<symbol>& Grammar::getFollow(symbol S){
    if(Follow.empty()) computeFirstAndFollow();
    return Follow[S];
}
const std::unordered_map<char,std::unordered_set<symbol>>& Grammar::allFirst(){
    if(First.empty()) computeFirstAndFollow();
    return First;
}
const std::unordered_map<char,std::unordered_set<symbol>>& Grammar::allFollow(){
    if(First.empty()) computeFirstAndFollow();
    return Follow;
}
const std::unordered_set<symbol>& Grammar::getTerminalSet(){
    return terminalSet;
}
const std::unordered_set<symbol>& Grammar::getNonTerminalSet(){
    return nonTerminalSet;
}
const std::unordered_map<symbol,Production>& Grammar::getProductions(){
    return productions;
}
symbol Grammar::getStart(){
    return Start;
}
symbol Grammar::changeStart(){
    auto newStart = getNewNonTerminal();
    productions[newStart].push_back({Start});
    Start = newStart;
    return newStart;
}

Grammar::Grammar(symbol start, std::vector<std::string> productionStrs) {
    Start = start;
    for(auto& productionStr:productionStrs){
        nonTerminalSet.insert(productionStr[0]);
    }
    if(nonTerminalSet.count(start) == 0){
        error("起始符不存在于非终结符中");
        return;
    }
    for (auto& productionStr:productionStrs) {
        auto&& thisProduction = productions[productionStr[0]];
        std::vector<char> sub;
        for(int i = 2;i<productionStr.size();i++){
            if(productionStr[i] == '|'){
                thisProduction.push_back(sub);
                sub.clear();
            }else{
                if(productionStr[i] == '-')
                    sub.push_back('\0');
                else{
                    if(productionStr[i]<'A'||productionStr[i]>'Z'){
                        terminalSet.insert(productionStr[i]);
                    }
                    if(!sub.empty()&&sub.back() == '\0') sub.pop_back();
                    sub.push_back(productionStr[i]);
                }
            }
        }
        if(!sub.empty()) thisProduction.push_back(sub);
    }
}


#endif