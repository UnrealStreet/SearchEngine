/**
  ******************************************************************************
  * @file           : WebInfo.h
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/21
  ******************************************************************************
  */

#ifndef SEARCHENGINE_WEBINFO_H
#define SEARCHENGINE_WEBINFO_H

#include <unordered_map>
#include <set>
#include <string>

using std::unordered_map;
using std::set;
using std::pair;
using std::string;

class WebInfo {
public:
    void loadInfoFromFile(const string &offsetFilePath, const string &invertIndexFilePath);

    static WebInfo *getInstance();

    const unordered_map<int, pair<int, int>> &getOffsetLib() const;

    unordered_map<int, pair<int, int>> &getOffsetLib();

    const unordered_map<string, set<pair<int, double>>> &getInvertIndexLib() const;

    unordered_map<string, set<pair<int, double>>> &getInvertIndexLib();

    WebInfo(const WebInfo &) = delete;

    WebInfo &operator=(const WebInfo &) = delete;
private:
    WebInfo();

    ~WebInfo();


private:
    unordered_map<int, pair<int, int>> _offsetLib;
    unordered_map<string, set<pair<int, double>>> _invertIndexLib;

};


#endif //SEARCHENGINE_WEBINFO_H
