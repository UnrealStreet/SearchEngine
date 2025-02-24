/**
  ******************************************************************************
  * @file           : testEditDistance.cc
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/14
  ******************************************************************************
  */
#include "KeyRecommender.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <word>" << endl;
        return 1;
    }
    KeyRecommender keyRecommender(argv[1]);
    for (int i = 2; i < argc; i++) {
        string word = argv[i];
        int distance = keyRecommender.editDistance(word);
        cout << word << ": " << distance << endl;
    }
    return 0;
}