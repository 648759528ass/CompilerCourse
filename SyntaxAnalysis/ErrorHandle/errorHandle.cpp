//
// Created by jhas on 2022/6/13.
//
#include <iostream>
#include <string>
#include "errorHandle.h"
Error::Error(){
    errorCount = 0;
}
void error(std::string msg){
    auto& errorInst = Error::getError();
    errorInst.writeLog(msg);
}