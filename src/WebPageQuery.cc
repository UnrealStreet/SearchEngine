#include <fstream>
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "WebPageQuery.h"
#include "SplitTool.h"
#include "Configuration.h"
#include <wfrest/json.hpp>
#include "WebInfo.h"

using std::ifstream;
using std::istringstream;
using std::cerr;
using std::cout;
using std::ios;
using std::set_intersection;

WebPageQuery::WebPageQuery(SplitTool *Tool) :
        _wordCutTool(Tool) {
}

//以下是基于 TF-IDF加权评分 的改进代码，支持按文档的TF-IDF累加值排序，并可选最低关键词匹配数：
//加权评分机制
//双重过滤条件
//min_keywords：文档必须至少包含指定数量的查询关键词（默认2）。
//min_score：文档的TF-IDF总分需超过阈值（默认0，可根据数据分布调整）。
//两级排序规则:第一优先级：匹配关键词数量多的文档。第二优先级：TF-IDF总分高的文档。
//时间复杂度遍历所有查询词的倒排列表，时间复杂度为 O(Q * L)，其中：Q：查询词数量 L：单个倒排列表的平均长度
vector<int> WebPageQuery::findRelevantDocumentsWeighted(
        const vector<string> &query_words,
        const InvertedIndex &invertIndexLib,
        double min_score    // 最低TF-IDF总分阈值
) {
    // 文档ID → {匹配关键词数, TF-IDF总分}
    unordered_map<int, pair<int, double>> doc_scores;
    int min_keywords = std::max(2, static_cast<int>(query_words.size() * 0.5));//根据查询词数量自动调整min_keywords
    cout << "min_keywords: " << min_keywords << "\n";
    // 遍历查询词，累加评分
    for (const auto &word: query_words) {
        auto it = invertIndexLib.find(word);
        if (it == invertIndexLib.end()) continue;

        // 遍历当前词的倒排列表
        for (const auto &[doc_id, tfidf]: it->second) {
            auto &score = doc_scores[doc_id];
            score.first++;          // 匹配关键词数+1
            score.second += tfidf;  // 累加TF-IDF值
        }
    }

    // 转换为vector并过滤
    vector<pair<int, pair<int, double>>> scored_docs;
    for (const auto &[doc_id, score]: doc_scores) {
        if (score.first >= min_keywords && score.second >= min_score) {
            scored_docs.emplace_back(doc_id, score);
        }
    }

    // 排序规则：优先按匹配数降序，其次按TF-IDF总分降序
    sort(scored_docs.begin(), scored_docs.end(),
         [](const auto &a, const auto &b) {
             if (a.second.first != b.second.first) {
                 return a.second.first > b.second.first;  // 匹配数优先
             }
             return a.second.second > b.second.second;    // 总分其次
         });

    // 提取排序后的文档ID
    vector<int> result;
    for (const auto &[doc_id, _]: scored_docs) {
        result.push_back(doc_id);
    }

    return result;
}
//找到包含所有关键词的文档id(交集)   使用的第一种方法
/*set<int> findRelevantDoc( const vector<string>& words, const InvertedIndex& invertIndexLib) {
    set<int> docIdSet;//存放包含所有关键词的文档id
    for (auto &&word: words) {//遍历分词结果
        auto &&iter = invertIndexLib.find(word);//查找倒排索引
        if (iter != invertIndexLib.end()) {//找到了
            if (docIdSet.empty()) {//第一次找到
                for (auto &&elem: iter->second) {//将所有文档id插入
                    docIdSet.insert(elem.first);//取并集，里面都是候选词的下标
                }
                cout << "docIdSet size: " << docIdSet.size() << "\n";
            } else {//第二次找到
                set<int> temp;//存放当前关键词的文档id
                for (auto &&elem: iter->second) {
                    temp.insert(elem.first);
                }
                set<int> intersect;//存放交集
                set_intersection(docIdSet.begin(), docIdSet.end(), temp.begin(), temp.end(),
                                 inserter(intersect, intersect.begin()));
                docIdSet = std::move(intersect);//更新docIdSet
                cout << "交集大小: " << "\n";
                cout << "docIdSet size: " << docIdSet.size() << "\n";
            }
        }
    }
}*/


//去掉停用词 方法1：手动管理迭代器（适用于所有容器）
/*    for (auto it = words.begin(); it != words.end();) {
        if (stopWords.count(*it)) {
            it = words.erase(it);//!!!!!更新迭代器
        } else {
            ++it;
        }
    }*/
string WebPageQuery::doQuery(const string &key) {
    //1.准备工作
    auto &&offsetLib = WebInfo::getInstance()->getOffsetLib();
    auto &&invertIndexLib = WebInfo::getInstance()->getInvertIndexLib();
    auto &&words = _wordCutTool->cut(key);
    auto &&stopWords = Configuration::getInstance()->getStopWords();
        //去掉停用词 方法2：使用 erase-remove 惯用法（适用于序列容器）
        //erase-remove 惯用法比手动循环更高效，尤其对 std::vector 等连续存储容器
    words.erase(std::remove_if(words.begin(), words.end(),
                               [&stopWords](const auto &word) {
                                   return stopWords.count(word) > 0;
                               }), words.end());
    //计算余弦相似度并排序：相关性从高到低
    DocResult result;
    //2.计算余弦相似度
    CalculateAndStore(words, invertIndexLib, result);

    cout << "priority_queue size: " << result.size() << "\n";
    //3.返回结果
    string PageLibPath = Configuration::getInstance()->getConfigMap()["PageLib"] + "/pageLib.xml";
    ifstream ifs(PageLibPath);
    if (!ifs.is_open()) {
        cerr << "doQuery: file :" << PageLibPath << " open failed\n";
        return "";
    }
    using Json = nlohmann::json;
    Json message;
    int docId, pos, length;
    string txt;
    WebPage page;
    int index = 1;
    message["result"] = Json::array();
    while (!result.empty() && index <= 10) {
        auto &&elem = result.top();
        docId = elem.second;
        pos = offsetLib[docId].first;
        length = offsetLib[docId].second;
        ifs.seekg(pos);
        txt.resize(length);
        ifs.read(&txt[0], length);
        page.ParseDoc(txt);
        string name("WebPage");
        name += std::to_string(index);
        message["result"][index - 1]["title"] = page._docTitle;
        message["result"][index - 1]["content"] = page._docContent;
        message["result"][index - 1]["url"] = page._docLink;
        message["result"][index - 1][name] = page._docId;
        ++index;
        result.pop();
    }
    return message.dump(4);
}
//Json创建的另外一种方式
/*while (!result.empty() && index <= 10) {
    Json web;
    auto &&elem = result.top();
    docId = elem.second;
    pos = offsetLib[docId].first;
    length = offsetLib[docId].second;
    ifs.seekg(pos);
    txt.resize(length);
    ifs.read(&txt[0], length);
    page.ParseDoc(txt);
    string name("WebPage");
    name += std::to_string(index);
    web[name] = page._docId;
    web["content"] = page._docContent;
    web["title"] = page._docTitle;
    web["url"] = page._docLink;
    message["result"].push_back(web);
    ++index;
    result.pop();
}*/

void WebPageQuery::CalculateAndStore(vector<string> &words, const InvertedIndex &invertIndexLib, DocResult &result) {
    //计算搜索词权重
    map<string, double> wordFreq;
    unordered_map<string, double> searchWeight;
    double IDF = log2(1.5);
    for (auto &&word: words)
        ++wordFreq[word];
    for (auto &&elem: wordFreq) {
        elem.second = elem.second / static_cast<double >(words.size());//计算原始词频
        searchWeight[elem.first] = elem.second * IDF;
    }
    //L2范数归一化
    double sum = 0.0;
    for (auto &&elem: searchWeight)
        sum += elem.second * elem.second;
    sum = sqrt(sum);
    for (auto &&elem: searchWeight)
        elem.second /= sum;
    //打印搜索词权重
    cout << "搜索词权重:" << "\n";
    for (auto &&elem: searchWeight) {
        cout << elem.first << " " << elem.second << "\n";
    }

    //1.对分词结果进行处理
    auto &&docIdSet = findRelevantDocumentsWeighted(words, invertIndexLib);

    //2.计算余弦相似度
    for (auto &&docId: docIdSet) {//遍历包含所有关键词的文档id
        double dotProduct = 0.0;
        for (auto &&elem: searchWeight) {
            auto &&iter = invertIndexLib.find(elem.first);
            if (iter != invertIndexLib.end()) {//倒排索引存在该词
                auto &&iter2 = iter->second.lower_bound({docId, 0.0});//找到文档id对应的权重
                if (iter2 != iter->second.end() && iter2->first == docId) {//找到对应文档中该词的权重
                    dotProduct += elem.second * iter2->second;
                }
            }
        }
        result.emplace(dotProduct, docId);
    }
}
