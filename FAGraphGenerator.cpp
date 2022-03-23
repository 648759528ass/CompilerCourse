#include "utils/utils.h"
int main(int argv,char *argc[]) {
    if(argv<3){
        printf("usage:%s <basename> <regex>\n",argc[0]);
        exit(0);
    }
    utils::generateTotal(argc[1],argc[2]);
}