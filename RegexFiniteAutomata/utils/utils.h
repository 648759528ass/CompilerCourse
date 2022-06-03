//
// Created by Mac on 23/03/2022.
//

#ifndef COMPILERCOURSE_UTILS_H
#define COMPILERCOURSE_UTILS_H
#include "../FiniteAutomata/NFA.h"
#include "../FiniteAutomata/DFA.h"

class utils {
public:
    static void generateGraphViz(std::string fileName,std::string body);
    static void generateTotal(std::string baseName,std::string regex);
};


#endif //COMPILERCOURSE_UTILS_H
