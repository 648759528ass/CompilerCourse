#include <iostream>
#include <string>
#include "FiniteAutomata/NFA.h"
#include "FiniteAutomata/DFA.h"
int main() {
    std::string regex = "(a|b)*cd|j*";
    NFA ret = NFA::makeNFAFromRegex(regex);
//    std::cout<<ret.getStart();
//    std::cout<<ret;
    DFA D(ret);
    std::cout<<D;
}
