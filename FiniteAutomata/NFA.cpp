//
// Created by Mac on 16/03/2022.
//

#include "NFA.h"
#include <stack>
std::string RegexHandler::addConcat(std::string s){
    std::string ret;
    for(int i = 0;i<s.size();i++){
        if(s[i] == '\\'){
            i++;
        }
        if(i != s.size()-1&&s[i+1] != ')'&&s[i+1] != '|'&&s[i]!='|'&&s[i]!='('&&s[i+1]!='*'){
            s.insert(i+1,"&");
            i++;
        }
    }
    return s;
}
std::string RegexHandler::inToSuffix(std::string s){
    std::stack<std::string> charST;
    std::stack<char> operatorST;
    std::string ret;
    int n = s.size();
    for(int i = 0;i<n;i++){
        char c = s[i];
        switch (c) {
            case '(':{
                operatorST.push(c);
                break;
            }
            case ')':{
                while (operatorST.top()!='('){
                    auto top = operatorST.top();
                    operatorST.pop();
                    std::string right = charST.top();
                    charST.pop();
                    std::string left = charST.top();
                    charST.pop();
                    charST.push(left + right + top);
                }
                operatorST.pop();
                break;
            }
            case '|':{
                while(!operatorST.empty()&&(operatorST.top() == '&'|| operatorST.top() == '|')){
                    auto top = operatorST.top();
                    operatorST.pop();
                    std::string right = charST.top();
                    charST.pop();
                    std::string left = charST.top();
                    charST.pop();
                    charST.push(left + right + top);
                }
                operatorST.push('|');
                break;
            }
            case '*':{
                auto top = charST.top();
                charST.pop();
                charST.push(top+'*');
                break;
            }
            case '&':{
                while(!operatorST.empty()&&operatorST.top() == '&'){
                    auto top = operatorST.top();
                    operatorST.pop();
                    std::string right = charST.top();
                    charST.pop();
                    std::string left = charST.top();
                    charST.pop();
                    charST.push(left + right + top);
                }
                operatorST.push('&');
                break;
            }
            case '\\':{
                if(i+1<s.size())
                    charST.push("\\"+std::string(1,s[i+1]));
                i++;
                break;
            }
            default:{
                charST.push(std::string(1,c));
            }
        }
    }
    while (!operatorST.empty()){
        auto c = operatorST.top();
        switch (c) {
            case '(': throw "error";
            default:{
                auto top = operatorST.top();
                operatorST.pop();
                std::string right = charST.top();
                charST.pop();
                std::string left = charST.top();
                charST.pop();
                charST.push(left + right + top);
            }
        }
    }
    if(charST.size() > 1) throw "error charcost";
    return charST.top();
}

NFA NFA::makeNFAFromSuffix(std::string s){
    std::stack<NFA> st;
    for(int i = 0;i<s.length();i++){
        switch (s[i]) {
            case '|':{
                auto b = st.top();
                st.pop();
                auto a = st.top();
                st.pop();
                st.push(NFA::uni(a,b));
                break;
            }
            case '&':{
                auto b = st.top();
                st.pop();
                auto a = st.top();
                st.pop();
                st.push(NFA::concat(a,b));
                break;
            }
            case '*':{
                auto a = st.top();
                st.pop();
                st.push(NFA::closure(a));
                break;
            }
            case '\\':{
                i++;
            }
            default:{
                st.push(NFA(s[i]));
            }
        }
    }
    return st.top();
}
NFA NFA::makeNFAFromRegex(std::string s){
    return makeNFAFromSuffix(RegexHandler::inToSuffix(RegexHandler::addConcat(s)));
}
int NFANode::NodeCnt = 0;
NFANode::NFANode():nodeIdx(NodeCnt++),status(NFANode::NodeStatus::normal) {
}
NFA::NFA(char c) {
    start = std::make_shared<NFANode>();
    end = std::make_shared<NFANode>();
    end->status = NFANode::final;
    start->status = NFANode::normal;
    start->path[c].push_back(end);
}
NFA::NFA(std::shared_ptr<NFANode> s, std::shared_ptr<NFANode> e):start(std::move(s)),end(std::move(e)) {}
NFA NFA::concat(NFA& a,NFA& b){
    a.end->status = NFANode::NodeStatus::normal;
    a.end->path['\0'].push_back(b.start);
    return NFA(a.start,b.end);
}
NFA NFA::uni(NFA& a,NFA& b){
    auto start = std::make_shared<NFANode>();
    auto end = std::make_shared<NFANode>();

    a.end->status = NFANode::normal;
    a.end->path['\0'].push_back(end);

    b.end->status = NFANode::normal;
    b.end->path['\0'].push_back(end);

    start->path['\0'].push_back(a.start);
    start->path['\0'].push_back(b.start);
    return NFA(start,end);
}
NFA NFA::closure(NFA& a){
    auto start = std::make_shared<NFANode>();
    auto end = std::make_shared<NFANode>();
    start->path['\0'].push_back(end);
    start->path['\0'].push_back(a.start);
    a.end->path['\0'].push_back(a.start);
    a.end->path['\0'].push_back(end);
    a.end->status = NFANode::normal;
    end->status = NFANode::final;
    return NFA(start,end);
}
std::pair<NFAMap,charset> NFA::getMoveList(){
    std::unordered_set<int> st;
    NFAMap ret;
    charset cst;
    std::queue<std::shared_ptr<NFANode>> Q;
    Q.push(start);
    st.insert(start->nodeIdx);
    while(!Q.empty()){
        auto top = Q.front();
        Q.pop();
        for(auto&[k,v]:top->path){
            if(k!='\0')
                cst.insert(k);
            for(auto& p:v){
                ret[top->nodeIdx][k].push_back(p->nodeIdx);
                if(!st.count(p->nodeIdx)){
                    st.insert(p->nodeIdx);
                    Q.push(p);
                }
            }
        }
    }
    return {ret,cst};
}
int NFA::getStart() {
    return start->nodeIdx;
}
int NFA::getEnd() {
    return end->nodeIdx;
}
std::ostream& operator<< (std::ostream& out,NFA& A){
    std::unordered_set<int> st;
    std::queue<std::shared_ptr<NFANode>> Q;
    Q.push(A.start);
    st.insert(A.start->nodeIdx);
    while(!Q.empty()){
        auto top = Q.front();
        Q.pop();
        out<<top->nodeIdx<<":\n";
        for(auto&[k,v]:top->path){
            if(k == '\0') out<<"epi: {";
            else out<<k<<": {";
            for(auto& p:v){
                out<<p->nodeIdx<<" ";
                if(!st.count(p->nodeIdx)){
                    st.insert(p->nodeIdx);
                    Q.push(p);
                }
            }
            out<<"}\n";
        }
    }
    return out;
}

