#include <iostream>
#include <fstream>
#include <sstream>
#include "PageLibPreprocessor.h"
#include "simhash/Simhasher.hpp"
#include "Configuration.h"
#include "tinyxml2.h"
#include "SplitTool.h"

using std::cout;
using std::cerr;
using std::ifstream;
using std::ofstream;

const char *const DICT_PATH = "../resource/dict/jieba.dict.utf8";
const char *const HMM_PATH = "../resource/dict/hmm_model.utf8";
const char *const IDF_PATH = "../resource/dict/idf.utf8";
const char *const STOP_WORD_PATH = "../resource/dict/stop_words.utf8";

static int curDocId = 0;//当前文档的id

PageLibPreprocessor::PageLibPreprocessor(SplitTool *tool)
        : _wordCutter(tool) {
    _offsetLib.reserve(5000);
}

//读取网页库的偏移库信息
void PageLibPreprocessor::readInfoFromFile(const string &offsetFilePath) {
    auto &dirMap = Configuration::getInstance()->getConfigMap();
    string offsetLibPath = dirMap["PageLib"] + "/" + offsetFilePath;
    ifstream offset(offsetLibPath);
    if (!offset.is_open()) {
        cerr << "readInfoFromFile: file :" << offsetLibPath << " open failed\n";
        return;
    }
    int docId, pos = 0, length = 0;
    while (offset >> docId >> pos >> length) {
        _offsetLib[docId] = std::make_pair(pos, length);
    }
    offset.close();
    cout << "offsetLib is load successfully\n";
}

//处理网页库
void PageLibPreprocessor::doProcess() {
    readInfoFromFile("offsetLib.dat");
    cutRedundantPage();
//    buildInvertIndexMap();
    storeOnDisk();
}

//将冗余的网页删除
void PageLibPreprocessor::cutRedundantPage() {
    auto &dirMap = Configuration::getInstance()->getConfigMap();
    string pageLibPath = dirMap["PageLib"] + "/pageLib.xml";
    ifstream xml(pageLibPath);
    if (!xml.is_open()) {
        cerr << "cutRedundantPage: file :" << pageLibPath << " open failed\n";
    }
    using namespace simhash;
    Simhasher sim(DICT_PATH, HMM_PATH, IDF_PATH, STOP_WORD_PATH);
    int docNums = static_cast<int>(_offsetLib.size());
    cout << "docNums: " << docNums << "\n";
    int pos, length;
    size_t topN = 6;
    uint64_t res = 0;
    //保存所有文档的指纹
    string txt;
    for (int i = 1; i <= docNums; ++i) {
        pos = _offsetLib[i].first;
        length = _offsetLib[i].second;
        xml.seekg(pos);
        txt.resize(length);
        xml.read(&txt[0], length);
        sim.make(txt, topN, res);
        _fingerPrint[i] = res;
    }
    cout << "offsetLib size: " << _offsetLib.size() << "\n";
    cout << "fingerPrint size: " << _fingerPrint.size() << "\n";
    cout << "simhash is done\n";
    //计算相似度并删除海明距离小于5的文档
    for (int i = 1; i < docNums; ++i) {
        if (_fingerPrint.find(i) == _fingerPrint.end()) {
            continue;
        }
        for (int j = i + 1; j <= docNums; ++j) {
            if (_fingerPrint.find(j) == _fingerPrint.end()) {
                continue;
            }
            if (Simhasher::isEqual(_fingerPrint[i], _fingerPrint[j], 5)) {
                _offsetLib.erase(j);
                _fingerPrint.erase(j);
            }
        }
        docNums = static_cast<int>(_offsetLib.size());
    }
    cout << "cutRedundantPage is done\n";
    cout << "offsetLib size: " << _offsetLib.size() << "\n";
    cout << "fingerPrint size: " << _fingerPrint.size() << "\n";
    xml.close();
}

void PageLibPreprocessor::buildInvertIndexMap() {
    readInfoFromFile("offsetNewLib.dat");
    auto &dirMap = Configuration::getInstance()->getConfigMap();
    string pageLibNewPath = dirMap["PageLib"] + "/pageLib.xml";
    ifstream xml(pageLibNewPath);
    if (!xml.is_open()) {
        cerr << "buildInvertIndexMap: file :" << pageLibNewPath << " open failed\n";
        return;
    }
    string txt;
    txt.reserve(1000000);//1M
    using namespace tinyxml2;
    XMLDocument doc;//xml解析对象
    int docId, pos, length;
    //TODO:先使用map存储，后续改为vector
    map<int, int> docAllWord;//存储每篇文档的总词数
    map<string, int> DF;//存储每个词的文档频率
    curDocId = _offsetLib.begin()->first;
    set<string> clearWords;
    map<int, set<string >> docWords;//存储每篇文档的词
    auto &&stopWords = Configuration::getInstance()->getStopWords();
    for (auto &&elem: _offsetLib) {
        docId = elem.first;
        pos = elem.second.first;
        length = elem.second.second;
        xml.seekg(pos);//定位到文档的起始位置
        txt.resize(length);//重新分配空间
        xml.read(&txt[0], length);//读取文档内容
        if (xml.gcount() != length) {
            cerr << "read docId: " << docId << " failed\n";
            continue;
        }
        doc.Parse(txt.c_str());//解析文档
        if (doc.ErrorID() != XML_SUCCESS) {
            cerr << "ErrorStr: " << doc.ErrorStr() << "\n";
            cerr << "ErrorName: " << doc.ErrorName() << "\n";
            return;
        }
        XMLElement *docNode = doc.FirstChildElement("doc");
        XMLElement *titleNode = docNode->FirstChildElement("title");
        XMLElement *contentNode = docNode->FirstChildElement("content");
        txt.clear();
        if (!titleNode->GetText())
            txt.append("没有标题");
        else
            txt.append(titleNode->GetText());

        if (!contentNode->GetText())
            txt.append("没有内容");
        else
            txt.append(":").append(contentNode->GetText());
        auto &&words = _wordCutter->cutForSearch(txt);//分词
        for (auto &&word: words) {//去停用词
            if (stopWords.find(word) == stopWords.end()) {// 不是停用词
                clearWords.insert(word);
            }
        }
        docAllWord[docId] = static_cast<int>(clearWords.size());//记录每篇文档的总词数
        calculate(DF, clearWords, docWords, docId);
        clearWords.clear();
//        doc.Clear();
    }
    cout << "calculate is done\n";
    calculateWeight(DF, docAllWord, docWords);
    cout << "buildInvertIndexMap is done\n";
    storeInvertIndex();
}

void PageLibPreprocessor::calculate
        (map<string, int> &DF, set<string> &wordsVec, map<int, set<string >> &docWords, int newDocId) {
    map<string, int> TF;//某个词在当前文档中的词频
    for (auto &&elem: wordsVec) {
        //统计每篇文档的词频
        ++TF[elem];
        docWords[newDocId].insert(elem);
        //统计每个词的文档频率，只记录第一次出现
        if (DF.find(elem) == DF.end()) {
            DF[elem] = 1;
        } else if (curDocId != newDocId) {//如果是新文档，且该词已经出现过
            ++DF[elem];
        }
    }
    for (auto &&elem: TF) {
        _termFrequency[elem.first].insert(std::make_pair(newDocId, elem.second));
    }
    curDocId = newDocId;
}

void
PageLibPreprocessor::calculateWeight(map<string, int> &DFMap, map<int,int> &docAllWord,
                                     map<int, set<string>> &docWords) {
    auto N = static_cast<double>(_offsetLib.size());//文档总数
    vector<string> words;//存储每篇文档的词
    vector<double> weights;//存储每篇文档的权重
    for (auto &&elem: docWords) {//遍历每篇文档的词
        auto docId = elem.first;//文档id
        for (auto &&word: elem.second) {
            int length = docAllWord[docId];//文档总词数
            words.reserve(length);
            weights.reserve(length);
            //找到该词在当前文档中的词频
            auto TF = _termFrequency[word][docId] / static_cast<double>(length);
            auto DF = DFMap[word];
            double weight = TF * log2((N / DF + 1) + 1);
            words.push_back(word);
            weights.push_back(weight);
        }
        //对文档向量进行 L2 归一化，即 w_normalized = w / ||w||
        double sum = 0;
        for (auto &&weight: weights) {
            sum += weight * weight;
        }
        sum = sqrt(sum);
        for (double &weight: weights) {
            weight /= sum;
        }
        //将文档向量存储到倒排索引中
        for (int i = 0; i < words.size(); ++i) {
            _invertIndexLib[words[i]].insert(std::make_pair(docId, weights[i]));
        }
        words.clear();//清空
        weights.clear();//清空
    }
    cout << "calculateWeight is done\n";
}

void PageLibPreprocessor::storeInvertIndex() {
    cout << "invertIndexLib size: " << _invertIndexLib.size() << "\n";
    auto &dirMap = Configuration::getInstance()->getConfigMap();
    string invertIndexLibPath = dirMap["InvertedIndexLib"] + "/invertIndexLib.dat";
    ofstream ofs(invertIndexLibPath);
    if (!ofs.is_open()) {
        cerr << "storeInvertIndex: file :" << invertIndexLibPath << " open failed\n";
        return;
    }
    std::ostringstream oss;
    for (auto &&elem: _invertIndexLib) {
        oss << elem.first;
        for (auto &&doc: elem.second) {
//            oss << " (" << doc.first << "," << doc.second << ")";便于查看
            oss << " " << doc.first << " " << doc.second;//便于存储和读取
        }
        oss << "\n";
    }
    ofs << oss.str();
    ofs.close();
    cout << "invertIndexLib store at: " << invertIndexLibPath << " is success\n";
}

//将网页库存储到磁盘
void PageLibPreprocessor::storeOnDisk() {
    //存储新的偏移库
    std::ostringstream oss;
    for (auto &&elem: _offsetLib) {
        oss << elem.first << " " << elem.second.first << " " << elem.second.second << "\n";
    }
    ofstream ofs("../data/offsetNewLib.dat");//存储新的偏移库
    ofs << oss.str();
    ofs.close();
    cout << "offsetNewLib store success\n";

    auto &dirMap = Configuration::getInstance()->getConfigMap();
    string offsetLibPath = dirMap["PageLib"] + "/offsetNewLib.dat";
    ifstream offset(offsetLibPath);//加载新的偏移库
    if (!offset.is_open()) {
        cerr << "readInfoFromFile: file :" << offsetLibPath << " open failed\n";
        return;
    }
    int docId, pos = 0, length = 0;
    while (offset >> docId >> pos >> length) {//读取新的偏移库
        _offsetLib[docId] = std::make_pair(pos, length);
    }
    offset.close();
    cout << "offsetNewLib is load successfully\n";
    cout << "offsetNewLib size: " << _offsetLib.size() << "\n";

//    auto &dirMap = Configuration::getInstance()->getConfigMap();
    string pageLibPath = dirMap["PageLib"] + "/pageLib.xml";
    ifstream page(pageLibPath);//读取原网页库
    if (!page.is_open()) {
        cerr << "storeOnDisk: file :" << pageLibPath << " open failed\n";
        return;
    }
//    int  pos, length;
    string pageLibNewPath = dirMap["PageLib"] + "/pageLibNew.xml";
    ofstream pageNew(pageLibNewPath);//新网页库
    string all;
    all.reserve(10000000);//10M
    if (!pageNew.is_open()) {
        cerr << "file :" << pageLibNewPath << " open failed\n";
        return;
    }
    unsigned long cur = 0;
    for (int i = 1; i <= 4400; ++i) {
        if (_offsetLib.find(i) == _offsetLib.end()) {//删除重复文档
            continue;
        }
        pos = _offsetLib[i].first;
        length = _offsetLib[i].second;
        page.seekg(pos);
        page.read(&all[0] + cur, length);
        cur += length;
        if (cur >= 9990000) {//大于
            pageNew.write(all.data(), cur);
            cur = 0;
        }
    }
    if (cur > 0) {
        pageNew.write(all.data(), cur);
    }
    page.close();
    pageNew.close();
    cout << "pageLibNew is store at: " << pageLibNewPath << " is success\n";
}