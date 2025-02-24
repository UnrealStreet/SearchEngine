#pragma once

#include <unordered_map>
#include <string>
#include <map>
#include <set>
#include <cstdint>
#include <vector>

using std::unordered_map;
using std::pair;
using std::string;
using std::map;
using std::set;
using std::vector;

class SplitTool;

class PageLibPreprocessor {
public:
    explicit PageLibPreprocessor(SplitTool *splitTool);

    //对网页库进行预处理
    void readInfoFromFile(const string &offsetFilePath);

    void doProcess();

    //对冗余的网页进行去重
    void cutRedundantPage();

    //创建倒排索引
    void buildInvertIndexMap();

    void storeOnDisk();

    void storeInvertIndex();

private:
    void calculate(map<string, int> &DTF, set<string> &wordsVec, map<int, set<string>> &docWords, int newDocId);

    void calculateWeight(map<string, int> &DF, map<int,int> &docAllWord, map<int, set<string>> &docWords);

private:
    map<int, std::uint64_t> _fingerPrint;
    unordered_map<int, pair<int, int>> _offsetLib;
    unordered_map<string, set<pair<int, double>>> _invertIndexLib;
    unordered_map<string, map<int, int>> _termFrequency;
    SplitTool *_wordCutter;
};

