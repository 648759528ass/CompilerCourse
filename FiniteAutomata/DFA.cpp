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
    std::unordered_map<std::set<int>,int,customHash> record;
    std::set<int> st;
    std::queue<std::set<int>> Q;
    st.insert(start);
    st = epsilon_closure(std::move(st),mp);
    record[st] = status++;
    edges.emplace_back();
    Q.push(st);
    while(!Q.empty()){
        auto top = Q.front();
        Q.pop();
        int this_stat = record[top];
        if(top.count(end))
            finalStatus.insert(this_stat);
        for(auto &c:cst){
            auto nextSt = epsilon_closure(move(top,c,mp),mp);
            if(nextSt.empty()) continue;
            int nxStatus;
            if(record.count(nextSt)){
                nxStatus = record[nextSt];
            }else{
                nxStatus = status;
                record[nextSt] = status++;
                edges.emplace_back();
                Q.push(nextSt);
            }
            edges[this_stat][c] = nxStatus;
        }
    }
    minor(cst);

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
        for(auto&[k,v]:A.edges[i]){
            out<<" "<<k<<"->"<<v<<" ";
        }
        out<<"}\n";
    }
    return out;
}
std::set<int> DFA::epsilon_closure(std::set<int> &&src, NFAMap &mp)
{
    std::set<int> ret;
    for(auto& s:src){
        ret.insert(s);
        if(mp[s].count('\0')){
            ret.insert(mp[s]['\0'].begin(),mp[s]['\0'].end());
        }
    }
    if(ret == src) return ret;
    return epsilon_closure(std::move(ret),mp);
}
std::set<int> DFA::move(std::set<int> src, char by, NFAMap &mp)
{
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
        if(edges[st].find(c)!=edges[st].end()){
            st = edges[st][c];
        }else return false;
    }
    return finalStatus.count(st);
}
void DFA::minor(charset& cst){
    std::unordered_set<std::set<int>, customHash> stateGroup, newStateGroup;
    std::set<int> noramlStatus,finalStatus;
    for(int i = 0;i<status;i++){
        if(this->finalStatus.count(i)){
            finalStatus.insert(i);
        }else{
            noramlStatus.insert(i);
        }
    }
    newStateGroup.insert(noramlStatus);
    newStateGroup.insert(finalStatus);
    do{
        stateGroup = newStateGroup;
        newStateGroup.clear();
        std::unordered_map<std::set<int>, std::set<int>, customHash> record;
        std::for_each(stateGroup.begin(), stateGroup.end(), [&record](auto& st) {
            record[st] = {};
        });
        std::vector<std::set<int>> store;
        int brkSignal = 0;
        for (auto &v : stateGroup){
            if(brkSignal){
                newStateGroup.insert(v);
                continue;
            }
            for (auto &c : cst){
                for (auto &sta : v){
                    for(auto&[k,v]:record){
                        if(k.count(edges[sta][c])){
                            v.insert(sta);
                        }
                    }
                }
                for(auto&[k,v]:record)
                {
                    if(v.size()){
                        store.push_back(v);
                        v.clear();
                    } 
                };
                if(store.size()>1){
                    brkSignal = 1;
                    for(auto& v:store){
                        newStateGroup.insert(v);
                    }
                }
                store.clear();
            }
            if(!brkSignal){
                newStateGroup.insert(v);
            }
        }
    }while(newStateGroup != stateGroup);
    Edges newEdges;                  // 新边集
    std::unordered_map<int, int> leaderToNewNode,nodeToLeader; // 领导节点对应新边集点
    //并查集开始
    std::unordered_map<int, int> mergeSet;
    for(int i = 0;i<status;i++) mergeSet[i] = i;
    std::function<int(int)> lookup;
    lookup = [&mergeSet,&lookup](int a)->int{
        return a == mergeSet[a] ? a : mergeSet[a] = lookup(mergeSet[a]);
    };
    auto merge = [&mergeSet,&lookup](int a,int b){
        auto fa = lookup(a);
        auto fb = lookup(b);
        mergeSet[fb] = fa;
    };
    //并查集结束
    int statCnt = 0;
    int newStart = 0;
    std::unordered_set<int> newFinalStatus;
    for (auto &st:newStateGroup){
        if(st.count(0)) newStart = statCnt;
        int leader = *(st.begin());
        if( finalStatus.count(leader) ) newFinalStatus.insert(statCnt);
        for(auto &v:st){
            merge(leader,v);
        }
        leaderToNewNode[leader] = statCnt;
        nodeToLeader[statCnt++] = leader;
    }
    for(int i = 0;i<statCnt;i++){
        std::unordered_map<char, int> edge;
        int srcNode = nodeToLeader[i];
        for(auto &c:cst){
            if(edges[srcNode].count(c)){
                edge[c] = leaderToNewNode[lookup(edges[srcNode][c])];
            }
        }
        newEdges.push_back(edge);
    }
    clearDeadNode(cst, newEdges, newStart, statCnt, newFinalStatus);
    status = statCnt;
    edges = newEdges;
    this->finalStatus = newFinalStatus;
}
bool checkFinal(
        int nowState,
        Edges &edges,
        std::unordered_set<int>& finalStatus,
        bool vis[],
        bool connectFinal[]
){
    if(connectFinal[nowState]) return true;
    if(finalStatus.count(nowState)) return true;
    for(auto &[k,v]:edges[nowState]){
        if(!vis[v]){
            vis[v] = true;
            if(connectFinal[v]||checkFinal(v,edges,finalStatus,vis,connectFinal)){
                return connectFinal[nowState] = true;
            }
            vis[v] = false;
        }
    }
    return false;
}
void DFA::clearDeadNode(
    charset &cst,
    Edges &edges,
    int start,
    int &status,
    std::unordered_set<int>& finalStatus
    )
{
    Edges newEdges;
    std::unordered_set<int> newFinalStatus;
    std::map<int,int> mp;
    std::queue<int> Q;
    bool vis[status],connectFinal[status];
    memset(vis,0,sizeof(vis));
    memset(connectFinal,0,sizeof(connectFinal));
    int cnt = 0;
    Q.push(start);
    newEdges.emplace_back();
    if (finalStatus.count(start))
        newFinalStatus.insert(cnt);
    mp[start] = cnt++;
    checkFinal(start,edges,finalStatus,vis,connectFinal);
    while(!Q.empty()){
        auto top = Q.front();
        Q.pop();
        for(auto &[k,v]:edges[top]){
            if(!(connectFinal[v]||checkFinal(v,edges,finalStatus,vis,connectFinal))) continue;
            if (mp.find(v)==mp.end()){
                if (finalStatus.count(v))
                    newFinalStatus.insert(cnt);
                newEdges.emplace_back();
                Q.push(v);
                mp[v] = cnt++;
            }
            newEdges[mp[top]][k] = mp[v];
        }
    }
    status = cnt;
    edges = newEdges;
    finalStatus = newFinalStatus;
}
