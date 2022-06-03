#include <iostream>
#include "FiniteAutomata/DFA.h"
#include "FiniteAutomata/NFA.h"
#include "utils/utils.h"
int main() {
//    std::cout<<RegexHandler::inToSuffix("(a|b)*ab(a*|b)");
    NFA N = NFA::makeNFAFromRegex("(a*|c*)d(a*|b)*ab*");
    DFA D(N);
    D.minor();
    std::cout<<D.getGraphvizString();
}