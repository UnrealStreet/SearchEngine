/**
  ******************************************************************************
  * @file           : WebInfo.cc
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/21
  ******************************************************************************
  */

#include "WebInfo.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Configuration.h"

using std::ifstream;
using std::istringstream;
using std::cerr;
using std::cout;
using std::ios;


WebInfo::WebInfo() {
    auto &dirMap = Configuration::getInstance()->getConfigMap();
    string offsetLibPath = dirMap["OffsetLib"] + "/offsetNewLib.dat";
    string invertIndexLibPath = dirMap["InvertedIndexLib"] + "/invertIndexLib.dat";
    loadInfoFromFile(offsetLibPath, invertIndexLibPath);
}

WebInfo::~WebInfo() = default;

WebInfo *WebInfo::getInstance() {
    static WebInfo instance;
    return &instance;
}

const unordered_map<int, pair<int, int>> &WebInfo::getOffsetLib() const {
    return _offsetLib;
}

unordered_map<int, pair<int, int>> &WebInfo::getOffsetLib() {
    return _offsetLib;
}

const unordered_map<string, set<pair<int, double>>> &WebInfo::getInvertIndexLib() const {
    return _invertIndexLib;
}

unordered_map<string, set<pair<int, double>>> &WebInfo::getInvertIndexLib() {
    return _invertIndexLib;
}

void WebInfo::loadInfoFromFile(const string &offsetFilePath, const string &invertIndexFilePath) {
    ifstream offset(offsetFilePath);
    if (!offset.is_open()) {
        cerr << "readInfoFromFile: file :" << offsetFilePath << " open failed\n";
        return;
    }
    int docId, pos = 0, length = 0;
    while (offset >> docId >> pos >> length) {
        _offsetLib[docId] = std::make_pair(pos, length);
    }
    offset.close();
    cout << "offsetLib : " << offsetFilePath << " is load successfully\n";
    //读取倒排索引
    ifstream invertIndex(invertIndexFilePath, ios::ate);
    if (!invertIndex.is_open()) {
        cerr << "readInfoFromFile: file :" << invertIndexFilePath << " open failed\n";
        return;
    }
    long size = invertIndex.tellg();
    invertIndex.seekg(0, ios::beg);
    string txt;
    txt.resize(size);
    invertIndex.read(&txt[0], size);
    if(invertIndex.gcount() != size)
    {
        cerr << "readInfoFromFile: file :" << invertIndexFilePath << " read failed\n";
        return;
    }
    istringstream iss(txt);//将读取的内容转换为字符串流,减少磁盘IO次数
    string word;
    string line;
    docId = 0;
    double weight = 0.0;
    istringstream lineStream;//将每一行的内容转换为字符串流
    while (getline(iss, line)) {
        lineStream.clear();
        lineStream.str(line);
        lineStream >> word;
        while (lineStream >> docId >> weight) {
            _invertIndexLib[word].insert(std::make_pair(docId, weight));
        }
    }
    cout << "invertIndexLib size: " << _invertIndexLib.size() << "\n";
    invertIndex.close();
    cout << "invertIndexLib : " << invertIndexFilePath << " is load successfully\n";
}