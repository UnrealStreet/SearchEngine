#include "CandidateResult.h"

CandidateResult::CandidateResult(const string &word, int freq, int dist)
        : _word(word),
          _freq(freq),
          _dist(dist) {
}

bool CandidateResult::operator<(const CandidateResult &rhs) const {
    if (_dist != rhs._dist) {
        return _dist < rhs._dist;//编辑距离越小，优先级越高
    } else if (_freq != rhs._freq) {
        return _freq > rhs._freq;//词频越高，优先级越高
    } else {
        return _word.size() > rhs._word.size();//字典序越小，优先级越高
    }
}