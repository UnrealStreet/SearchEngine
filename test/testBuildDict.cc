/**
  ******************************************************************************
  * @file           : testBuildDict.cc
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/9
  ******************************************************************************
  */

#include "DictProducer.h"
#include "SplitToolCppJieba.h"
#include <iostream>
#include <ctime>

int main() {
    //计算花费的毫秒数
    clock_t start, finish;
    start = clock();

    SplitToolCppJieBa splitTool;
    DictProducer dictProducer(&splitTool);

    dictProducer.buildEnDict();
    dictProducer.storeDict("../data/dictEn.dat");

    dictProducer.buildCnDict();
    dictProducer.storeDict("../data/dictCn.dat");

    dictProducer.buildIndex();
    dictProducer.storeIndex("../data/DictIndex.dat");
    finish = clock();
    std::cout << "Time cost: " << (double)(finish - start)/CLOCKS_PER_SEC << "s" << std::endl;
    return 0;
}