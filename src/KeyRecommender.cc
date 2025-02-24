#include "KeyRecommender.h"
#include <utility>
#include "Dictionary.h"
#include "wfrest/json.hpp"
#include <iostream>


KeyRecommender::KeyRecommender(string queryWord) : _queryWord(std::move(queryWord)) {
    splitWord();
}


int triple_min(const int &a, const int &b, const int &c) {
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}


void KeyRecommender::doQuery() {
    const auto &dict = Dictionary::getInstance()->getDict();
    const auto &index = Dictionary::getInstance()->getIndex();
    //遍历分词结果，并将每个分词结果所在的集合取一个并集
    set<int> result;
    for (const auto &word: _splitWord) {
        auto iter = index.find(word);
        if (iter != index.end()) {//找到了
            for (const auto &idx: iter->second) {
                result.insert(idx);//取并集，里面都是候选词的下标
            }
        }
    }
//    std::cout << "union size: " << result.size() << "\n";

    //遍历result，计算每个候选词与查询词的编辑距禽
    int maxDist = 0;
    for (const auto &idx: result) {
        const auto &candidate = dict[idx].first;//候选词
        int editDist = editDistance(candidate);//计算编辑距离
        if (_result.size() < 10) {
            maxDist = std::max(maxDist, editDist);
            //根据编辑距离越小，词频越大的原则，将候选词放入优先级队列，只保存前10个
            _result.emplace(candidate, dict[idx].second, editDist);
        } else {
            if (editDist <= maxDist) {
                _result.pop();//弹出编辑距离最大的
                _result.emplace(candidate, dict[idx].second, editDist);
                maxDist = _result.top()._dist;//更新最大编辑距禽
            }
        }
    }

//    std::cout << "maxDist: " << maxDist << "\n";
//    std::cout << "query result size: " << _result.size() << "\n";
}

string KeyRecommender::getCandidateWord()  {
    using Json = nlohmann::json;
    vector<CandidateResult> result;
    result.reserve(_result.size());
    while (!_result.empty()) {
        const auto &candidate = _result.top();
//        std::cout << candidate._word << " " << candidate._dist << " " << candidate._freq << "\n";
        result.emplace_back(candidate);
        _result.pop();
    }
    std::reverse(result.begin(), result.end());
    Json message;
    for (const auto &candidate: result) {
        message["query"].push_back(candidate._word);
    }
//    std::cout << message.dump() << std::endl;
    return message.dump(4);
}

//计算最小编辑距离-包括处理中英文
int KeyRecommender::editDistance(const string &rhs) {
    size_t lhs_len = length(_queryWord);
    size_t rhs_len = length(rhs);
    int editDist[lhs_len + 1][rhs_len + 1];
    for (size_t idx = 0; idx <= lhs_len; ++idx) {
        editDist[idx][0] = idx;
    }
    for (size_t idx = 0; idx <= rhs_len; ++idx) {
        editDist[0][idx] = idx;
    }
    string sublhs, subrhs;
    for (std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i,
            ++lhs_idx) {
        size_t nBytes = getByteNum_UTF8(_queryWord[lhs_idx]);
        sublhs = _queryWord.substr(lhs_idx, nBytes);
        lhs_idx += (nBytes - 1);
        for (std::size_t dist_j = 1, rhs_idx = 0;
             dist_j <= rhs_len; ++dist_j, ++rhs_idx) {
            nBytes = getByteNum_UTF8(rhs[rhs_idx]);
            subrhs = rhs.substr(rhs_idx, nBytes);
            rhs_idx += (nBytes - 1);
            if (sublhs == subrhs) {
                editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j -
                                                                1];
            } else {
                editDist[dist_i][dist_j] =
                        triple_min(editDist[dist_i][dist_j - 1] + 1,
                                   editDist[dist_i - 1][dist_j] + 1,
                                   editDist[dist_i - 1][dist_j - 1] + 1);
            }
        }
    }
    return editDist[lhs_len][rhs_len];
}

void KeyRecommender::splitWord() {
    int length = static_cast<int>(_queryWord.size());
    for (size_t idx = 0, n = 0; n < length; ++idx) {
        size_t charLen = getByteNum_UTF8(_queryWord[idx]);
        string subWord = _queryWord.substr(idx, charLen);
        idx += (charLen - 1);
        _splitWord.emplace_back(std::move(subWord));
        n += charLen;
    }
}

// 计算UTF-8编码的字符ch的字节数
size_t KeyRecommender::getByteNum_UTF8(const char byte) {
    int byteNum = 0;
    for (size_t i = 0; i < 6; ++i) {
        if (byte & (1 << (7 - i)))
            ++byteNum;
        else
            break;
    }
    return byteNum == 0 ? 1 : byteNum;
}

size_t KeyRecommender::length(const std::string &str) {
    std::size_t ilen = 0;
    for (std::size_t idx = 0; idx != str.size(); ++idx) {
        int nBytes = (int)getByteNum_UTF8(str[idx]);
        idx += (nBytes - 1);
        ++ilen;
    }
    return ilen;
}



