//
// Created by Mac on 16/03/2022.
//

#ifndef REGEX2DFA_NFA_H
#define REGEX2DFA_NFA_H
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>
#include <set>
#include <utility>
#include <memory>

using NFAMap = std::unordered_map<int,std::unordered_map<char,std::vector<int>>>;
using charset = std::unordered_set<char>;
class NFANode{
    static int NodeCnt ;
public:
    enum NodeStatus{
        normal,
        final
    };
    friend class NFA;
    int nodeIdx;
    NFANode();
    std::unordered_map<char,std::vector<std::shared_ptr<NFANode>>> path;
private:
    NodeStatus status;
};
class RegexHandler{
public:
    static std::string inToSuffix(std::string s);
    static std::string addConcat(std::string s);
};
class NFA {
private:
    std::shared_ptr<NFANode> start,end;
    NFA(std::shared_ptr<NFANode> s,std::shared_ptr<NFANode> e);
    static NFA concat(NFA& a,NFA& b);
    static NFA uni(NFA& a,NFA& b);
    static NFA closure(NFA& a);
    static NFA makeNFAFromSuffix(std::string s);



public:
    NFA(char  c);
    std::string getGraphvizString();
    static NFA makeUniversal();
    static NFA makeNFAFromRegex(std::string s);
    std::pair<NFAMap,charset> getMoveList();
    ~NFA(){
        if(start.use_count() == 1){
            std::queue<std::shared_ptr<NFANode>> Q;
            Q.push(start);
            while(!Q.empty()){
                auto top = Q.front();
                Q.pop();
                for(auto&[k,v]:top->path){
                    for(auto& p:v){
                        Q.push(p);
                    }
                    v.clear();
                }
            }
        }
    }
    int getStart();
    int getEnd();
    friend std::ostream& operator<< (std::ostream&,NFA& A);

};

#endif //REGEX2DFA_NFA_H
