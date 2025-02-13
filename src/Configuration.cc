#include "Configuration.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
Configuration::Configuration() {
    _configFilePath = "../conf/myConf.conf";
    ifstream ifs(_configFilePath);
    if (!ifs.is_open()) {
        cout << "Configuration::Configuration() ifstream open file error!" << endl;
        return;
    }
    string line;
    while (getline(ifs, line)) {
        istringstream iss(line);
        string key, value;
        iss >> key >> value;
//        _configs.insert(make_pair(key, value));
        _configs.emplace(key,value);
    }
    ifs.close();
}

set<string> Configuration::getStopWords() {
    set<string> ret;
    for (auto &&elem : _configs) {
        if (elem.first.substr(0, 9) == "StopWords") {
            ifstream ifs(elem.second,ios::ate);
            if (!ifs.is_open()) {
                cout << "Configuration::getStopWords() ifstream open file" << elem.second << " error!" << endl;
                return {};
            }
            long size = ifs.tellg();
            ifs.seekg(0, ios::beg);
            string stopWords;
            stopWords.resize(size);
            ifs.read(&stopWords[0], size);
            istringstream iss(stopWords);
            string word;
            while (iss >> word) {
                ret.insert(word);
            }
            ifs.close();
        }
    }
    return ret;
}

vector<string> Configuration::getDictFiles() {
    vector<string> ret;
    for (auto &&elem : _configs) {
        if (elem.first == "DictDir") {
            ret.push_back(elem.second);
        }
    }
    return std::move(ret);
}

Configuration *Configuration::getInstance() {
    static Configuration config;
    return &config;
}

map<string, string> &Configuration::getConfigMap() {
    return _configs;
}


