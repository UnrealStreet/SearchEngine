#ifndef _CANDIDATERESULT_H
#define _CANDIDATERESULT_H

#include <string>

using std::string;

class CandidateResult {
public:
    CandidateResult(const string &word, int freq, int dist);

    bool operator<(const CandidateResult &rhs) const;

public:
    string _word;//候选词
    int _freq;//词频
    int _dist;//与查询词的编辑距离
};

#endif //_CANDIDATERESULT_H