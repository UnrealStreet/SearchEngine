#include "DictProducer.h"
#include "SplitTool.h"
#include "Configuration.h"
#include "DirScanner.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <memory>

using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::ostringstream;
using std::cout;
using std::map;

DictProducer::DictProducer(SplitTool *tool) : _cuttor(tool) {
    _dict.reserve(10000);
    _stopWords = Configuration::getInstance()->getStopWords();
//    buildEnDict();
//    buildCnDict();
//    buildIndex();
}

//0.获取为英文文件 1.获取中文文件
void DictProducer::getFiles(int fileNum) {
    Configuration *conf = Configuration::getInstance();
    auto &&map = conf->getConfigMap();
    if (fileNum == 0) {
        string enDictDir = map["EnDictMetaDir"];
        DirScanner dirScanner;
        _files = dirScanner.getFiles(enDictDir);
//        for (auto &&elem: map) {
//            if (elem.first == "en_dict") {
//                _files.push_back(elem.second);
//            }
//        }
    } else {
        string cnDictDir = map["CnDictMetaDir"];
        DirScanner dirScanner;
        _files = dirScanner.getFiles(cnDictDir);
//        for (auto &&elem: map) {
//            if (elem.first == "cn_dict") {
//                _files.push_back(elem.second);
//            }
//        }
    }
}

string DictProducer::dealEnWord(string &word) {
    for (int i = 0; i < word.size(); ++i) {
        if (!isalpha(word[i]))
            return {};//不是字母，返回空字符串
        else if (isupper(word[i]))
            word[i] = static_cast<char>(tolower(word[i]));//转换为小写
    }
    return word;
}

void DictProducer::buildEnDict() {
    _files.clear();
    _dict.clear();
    getFiles(0);
    for (auto &&file: _files) {
        ifstream ifs(file, std::ios::ate);
        if (!ifs) {
            cout << "DictProducer::buildEnDict() ifstream open file : " << file << " error!" << "\n";
            return;
        }
        size_t fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        string txt;
        txt.resize(fileSize);
        ifs.read(&txt[0], fileSize);
//        char *buff = new char[fileSize + 1]();
//        ifs.read(buff, fileSize);
//        buff[fileSize] = '\0';
//        string txt(buff);
//        delete[] buff;
        istringstream iss(txt);
        string word;
        map<string, int> tempDict;
        while (iss >> word) {
            string newWord = dealEnWord(word);
            if (!_stopWords.count(newWord) && newWord.size() > 1)//去掉停用词和单个字符
                ++tempDict[newWord];
        }
        for (auto &&elem: tempDict) {
            _dict.emplace_back(elem);
        }
        ifs.close();
    }
    cout << "DictProducer::buildEnDict() build dict success!" << "\n";
}

void DictProducer::buildCnDict() {
    _files.clear();
    _dict.clear();
    getFiles(1);
    for (auto &&file: _files) {
        ifstream ifs(file, std::ios::ate);
        if (!ifs.is_open()) {
            cout << "DictProducer::buildCnDict() ifstream open file : " << file << " error!" << "\n";
            return;
        }
        long fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        string txt;
        txt.resize(fileSize);
        ifs.read(&txt[0], fileSize);
        //去掉换行符
        txt.erase(std::remove(txt.begin(), txt.end(), '\n'), txt.end());

        vector<string> words = _cuttor->cut(txt);
        map<string, int> tempDict;
        for (auto &&word: words) {
            if (!_stopWords.count(word) && getByteNum_UTF8(word[0]) > 1)//去掉停用词和单个字符
                ++tempDict[word];
        }
        for (auto &&elem: tempDict) {
            _dict.emplace_back(elem);
        }
        ifs.close();
    }
    cout << "DictProducer::buildCnDict() build dict success!" << "\n";
}

void DictProducer::buildIndex() {
    readDict();
//    _index.reserve(_dict.size() * 2);
    int i = 0; //记录下标
    for (auto &&elem: _dict) {
        string word = elem.first;
        size_t charNums = word.size() / getByteNum_UTF8(word[0]);
        for (size_t idx = 0, n = 0; n != charNums; ++idx, ++n)//得到字符数
        {
            //按照字符个数切割
            size_t charLen = getByteNum_UTF8(word[idx]);
            string subWord = word.substr(idx, charLen);//按照编码格式，进行拆分
            _index[subWord].insert(i);
            idx += (charLen - 1);
        }
        ++i;
    }
}

size_t DictProducer::getByteNum_UTF8(const char byte) {
    int byteNum = 0;
    for (size_t i = 0; i < 6; ++i) {
        if (byte & (1 << (7 - i)))
            ++byteNum;
        else
            break;
    }
    return byteNum == 0 ? 1 : byteNum;
}
//读取词典
void DictProducer::readDict() {
    vector<string> dictDir = Configuration::getInstance()->getDictFiles();
    vector<string> dictFiles;
    dictFiles.emplace_back(dictDir[0]+"/dictCn.dat");
    dictFiles.emplace_back(dictDir[0]+"/dictEn.dat");
    for (auto &&file: dictFiles) {
        ifstream ifs(file, std::ios::ate);
        if (!ifs.is_open()) {
            cout << "DictProducer::readDict() ifstream open file error!" << "\n";
            return;
        }
        long fileSize = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        string txt;
        txt.resize(fileSize);
        ifs.read(&txt[0], fileSize);
        istringstream iss(txt);
        string word;
        //只读取每一行的第一个单词
        while (iss >> word) {
            int freq;
            iss >> freq;
            _dict.emplace_back(word, freq);
        }
        ifs.close();
    }
}


void DictProducer::storeDict(const string &filepath) {
    ofstream ofs(filepath);
    if (!ofs) {
        cout << "DictProducer::store() ofstream open file error!" << "\n";
        return;
    }
    ostringstream oss;
    for (auto &&elem: _dict) {
        oss << elem.first << " " << elem.second << "\n";
    }
    ofs << oss.str();
    ofs.close();
    cout << "DictProducer::store() dict.dat store at:" << filepath << " success!" << "\n";
}

void DictProducer::storeIndex(const string &filepath) {
    ofstream ofs(filepath);
    if (!ofs) {
        cout << "DictProducer::store() ofstream open file error!" << "\n";
        return;
    }
    ostringstream oss;
    for (auto &&elem: _index) {
        oss << elem.first << " ";
        for (auto &&idx: elem.second) {
            oss << idx << " ";
        }
        oss << "\n";
    }
    ofs << oss.str();
    ofs.close();
    cout << "DictProducer::store() index.dat store at:" << filepath << " success!" << "\n";
}