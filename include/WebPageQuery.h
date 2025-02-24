#pragma once

#include <string>
#include <vector>
#include <set>
#include <queue>
#include <unordered_map>

#include "WebPage.h"

using std::string;
using std::vector;
using std::pair;
using std::set;
using std::unordered_map;
using std::priority_queue;

class SplitTool;

class WebPageQuery {
    using InvertedIndex = unordered_map<string, set<pair<int, double>>>;
    using DocResult = priority_queue<pair<double, int>>;
public:

    explicit WebPageQuery(SplitTool *Tool);

    string doQuery(const string &key);

private:
    // 从网页库中找到包含所有关键词的文档
    vector<int> findRelevantDocumentsWeighted(
            const vector<string> &query_words,
            const InvertedIndex &invertIndexLib,
            double min_score = 0.0    // 最低TF-IDF总分阈值
    );
    // 计算并存储文档的TF-IDF总分
    void CalculateAndStore(vector<string> &query_words, const InvertedIndex &invertIndexLib, DocResult &docResult);

    vector<WebPage> _pages;
    SplitTool *_wordCutTool;
};

