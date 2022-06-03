//
// Created by Mac on 23/03/2022.
//

#include "utils.h"
#include "../FiniteAutomata/NFA.h"
#include "../FiniteAutomata/DFA.h"
#include <string>
#include <fstream>
void utils::generateGraphViz(std::string fileName,std::string body){
    std::ofstream out(fileName+".dot");
    out<<"digraph G{\n"<<body<<"\n}";
    out.flush();
    out.close();
    std::string command = "dot -Tpng " + fileName+".dot" + " -o " + fileName+".png";
    system(command.c_str());
}
void utils::generateTotal(std::string baseName,std::string regex){
    NFA n = NFA::makeNFAFromRegex(regex);
    generateGraphViz(baseName+"-NFA",n.getGraphvizString());
    DFA d(n);
    generateGraphViz(baseName+"-DFA",d.getGraphvizString());
    d.minor();
    generateGraphViz(baseName+"-DFAMinor",d.getGraphvizString());
}