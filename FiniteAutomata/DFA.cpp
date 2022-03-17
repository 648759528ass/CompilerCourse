//
// Created by Mac on 17/03/2022.
//

#include "DFA.h"

std::size_t customHash::operator()(const std::set<int> &a) const {
    std::size_t seed = a.size();
    for(auto&i:a){
        seed ^= i+0x9e3779b9+(seed<<6) + (seed>>2);
    }
    return seed;
}

DFA::DFA(NFAMap& mp,charset& cst,int start,int end){
    create(mp,cst,start,end);
}
void DFA::create(NFAMap& mp,charset& cst,int start,int end) {
    this->mp = mp;
    std::unordered_map<std::set<int>,int,customHash> record;
    std::set<int> st;
    std::queue<std::set<int>> Q;
    st.insert(start);
    st = epsilon_closure(std::move(st));
    record[st] = status++;
    edge.emplace_back();
    Q.push(st);
    while(!Q.empty()){
        auto top = Q.front();
        Q.pop();
        int this_stat = record[top];
        if(top.count(end))
            finalStatus.insert(this_stat);
        for(auto &c:cst){
            auto nextSt = epsilon_closure(move(top,c));
            if(nextSt.empty()) continue;
            int nxStatus;
            if(record.count(nextSt)){
                nxStatus = record[nextSt];
            }else{
                nxStatus = status;
                record[nextSt] = status++;
                edge.emplace_back();
                Q.push(nextSt);
            }
            edge[this_stat][c] = nxStatus;
        }
    }
}
DFA::DFA(NFA& n){
    auto [moveList,cst] = n.getMoveList();
    create(moveList,cst,n.getStart(),n.getEnd());
}
std::ostream& operator<< (std::ostream& out,DFA& A){
    for(int i = 0;i<A.status;i++){
        out<<i<<" ";
        if(A.finalStatus.count(i)){
            out<<"(final) ";
        }
        out<<"\n{ ";
        for(auto&[k,v]:A.edge[i]){
            out<<" "<<k<<"->"<<v<<" ";
        }
        out<<"}\n";
    }
    return out;
}
std::set<int> DFA::epsilon_closure(std::set<int>&& src){
    std::set<int> ret;
    for(auto& s:src){
        ret.insert(s);
        if(mp[s].count('\0')){
            ret.insert(mp[s]['\0'].begin(),mp[s]['\0'].end());
        }
    }
    if(ret == src) return ret;
    return epsilon_closure(std::move(ret));
}
std::set<int> DFA::move(std::set<int> src,char by){
    std::set<int> ret;
    for(auto& s:src){
        if(mp[s].count(by)){
            ret.insert(mp[s][by].begin(),mp[s][by].end());
        }
    }
    return ret;
}
bool DFA::judge(std::string s){
    int st = 0;
    for(auto &c:s){
        if(edge[st].find(c)!=edge[st].end()){
            st = edge[st][c];
        }else return false;
    }
    return finalStatus.count(st);
}