#ifndef _KEYRECOMMANDER_H
#define _KEYRECOMMANDER_H

#include <string>
#include <queue>
#include <vector>
#include "CandidateResult.h"

using std::string;
using std::priority_queue;
using std::vector;


class KeyRecommender {
public:
    KeyRecommender(string queryWord);

    void doQuery();

    int editDistance(const string &rhs);

    string getCandidateWord();

private:
    void splitWord();

    size_t getByteNum_UTF8(const char ch);

    size_t length(const std::string &str);

private:
    string _queryWord;
    vector<string> _splitWord;
    priority_queue<CandidateResult> _result;
};

#endif //_KEYRECOMMANDER_H