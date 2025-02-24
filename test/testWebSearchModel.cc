/**
  ******************************************************************************
  * @file           : testWebSearchModel.cc
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/18
  ******************************************************************************
  */
#include "PageLibPreprocessor.h"
#include "SplitToolCppJieba.h"

int main(){
    SplitToolCppJieBa *jieBa = SplitToolCppJieBa::getInstance();
    PageLibPreprocessor pageLibPreprocessor(jieBa);
//    pageLibPreprocessor.doProcess();
    pageLibPreprocessor.buildInvertIndexMap();
    return 0;
}