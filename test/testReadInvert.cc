/**
  ******************************************************************************
  * @file           : testReadInvert.cc
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/21
  ******************************************************************************
  */

#include "WebInfo.h"
#include <iostream>

using namespace std;

int main() {
    WebInfo *webInfo = WebInfo::getInstance();
    auto &offsetLib = webInfo->getOffsetLib();
    auto &invertIndexLib = webInfo->getInvertIndexLib();

    return 0;
}