#include <iostream>
#include "FiniteAutomata/DFA.h"
#include "FiniteAutomata/NFA.h"
#include "utils/utils.h"
int main() {
    NFA n = NFA::makeNFAFromRegex("(a|b)*ab(a*|b)");
    utils::generateGraphViz("test",n.getGraphvizString());
}