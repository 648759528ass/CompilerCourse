#include <iostream>
#include "FiniteAutomata/DFA.h"

int main() {
    DFA D("(a|b)*ab(a*|b)");
    std::cout<<D.judge("aaabc");
}
