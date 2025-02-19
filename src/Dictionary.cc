#include <fstream>
#include <iostream>
#include "Dictionary.h"
#include <sstream>

using std::ifstream;
using std::cerr;
using std::cout;
using std::istringstream;

Dictionary * Dictionary::_pInstance = nullptr;
mutex Dictionary::_mutex;

Dictionary::Dictionary() {
    _dict.reserve(75000);
    initDict("../data/dictCn.dat");
    initDict("../data/dictEn.dat");
    initIndex("../data/DictIndex.dat");
}

Dictionary *Dictionary::getInstance() {
    _mutex.lock();
    if (_pInstance == nullptr) {
        _pInstance = new Dictionary();
    }
    _mutex.unlock();
    return _pInstance;
}

void Dictionary::destroy() {
    if (_pInstance) {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

Dictionary::~Dictionary() {
    //如果想用智能指针接管内存管理，需要将析构函数设为public
    //并在析构函数中设置_pInstance = nullptr，防止double free
//    _pInstance = nullptr;
}

void Dictionary::initDict(const string &dictPath) {
    ifstream dictFile(dictPath, std::ios::ate | std::ios::in);
    if (!dictFile.is_open()) {
        cerr << "open dict file: " << dictPath << " error\n";
        return;
    }
    long fileSize = dictFile.tellg();
    dictFile.seekg(0, std::ios::beg);
    string txt;
    txt.resize(fileSize);
    dictFile.read(&txt[0], fileSize);
    istringstream iss(txt);
    string word;
    int freq = 0;
    while (iss >> word >> freq) {
        _dict.emplace_back(word, freq);
    }
    dictFile.close();

//    cout << "dict file read success\n";
    //test if the  dict are read correctly
//    cout << "dict size: " << _dict.size() << "\n";
}

void Dictionary::initIndex(const string &indexPath) {

    ifstream indexFile(indexPath, std::ios::ate | std::ios::in);
    if (!indexFile.is_open()) {
        cerr << "open index file: " << indexPath << " error\n";
        return;
    }
    long fileSize = indexFile.tellg();
    indexFile.seekg(0, std::ios::beg);
    string txt;
    txt.resize(fileSize);
    indexFile.read(&txt[0], fileSize);
    istringstream iss(txt);
    string key;
    string line;
    int idx;
    while(getline(iss, line)) {
        istringstream lineIss(line);
        lineIss >> key;
        while (lineIss >> idx) {
            _index[key].insert(idx);
        }
    }
    indexFile.close();

//    cout << "index file read success\n";
//    cout << "index size: " << _index.size() << "\n";
}

vector<pair<string, int>> &Dictionary::getDict() {
    return _dict;
}

map<string, set<int>> &Dictionary::getIndex() {
    return _index;
}

vector<string> Dictionary::doQuery(const string &key) {
    return {};
}


void Dictionary::queryIndex() {

}



Dictionary::AutoRelease::AutoRelease() = default;

Dictionary::AutoRelease::~AutoRelease() {
    if (_pInstance) {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

Dictionary::AutoRelease Dictionary::_autoRelease;