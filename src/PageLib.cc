#include "PageLib.h"
#include "Configuration.h"
#include "tinyxml2.h"
#include <iostream>
#include <regex>
#include <fstream>
#include <sstream>

using std::cerr;
using std::cout;
using std::regex;
using std::string;
using std::ofstream;
using std::ostringstream;
using namespace ::tinyxml2;

string removeHTMLTags(const string &input) {
    // 定义一个正则表达式，同时匹配HTML标签和所有HTML实体（如&amp;、&lt;等）
//    regex htmlTagRegex("<[^>]*>");
    regex htmlTagRegex("<[^>]*>|\\&[^;]+;");
    // 使用 regex_replace 将标签替换为空字符串
    return regex_replace(input, htmlTagRegex, "");
}

PageLib::PageLib() {
}
//创建网页库 使用tinyXml2构造xml文件,但是有问题，输出的xml文件description内容与原文件不一致
/*void PageLib::create() {
    map<string, string> &conf = Configuration::getInstance()->getConfigMap();
    string xmlDir = conf["WebCorpusDir"];
    auto &files = _dirScanner.getFiles(xmlDir);
    XMLDocument doc;//xml解析对象
    XMLDocument *myDoc = new XMLDocument();
    int docNum = 1;
    int base = 96;
    unsigned long long pos = 0;
    unsigned long long length = 0;
    for (auto &&file: files) {
        doc.LoadFile(file.c_str());
        if (doc.ErrorID() != 0) {
            cerr << "ErrorStr: " << doc.ErrorStr() << "\n";
            cerr << "ErrorName: " << doc.ErrorName() << "\n";
            cerr << "ErrorID: " << doc.ErrorID() << "\n";

        }
        XMLElement *itemNode = doc.FirstChildElement("rss")
                ->FirstChildElement("channel")
                ->FirstChildElement("item");
        while (itemNode) {
            string title = itemNode->FirstChildElement("title")->GetText();
            string link = itemNode->FirstChildElement("link")->GetText();
            string content;
            if (itemNode->FirstChildElement("description") != nullptr)
                content = removeHTMLTags(itemNode->FirstChildElement("description")->GetText());
            else//没有description节点
                content = "no description";

            XMLElement *docNode = myDoc->NewElement("doc");
            myDoc->InsertEndChild(docNode);//插入一个doc节点
            //插入docId节点
            XMLElement *docId = myDoc->NewElement("docId");
            docId->SetText(docNum);
            docNode->InsertEndChild(docId);
            //插入title节点
            XMLElement *docTitle = myDoc->NewElement("title");
            docTitle->SetText(title.c_str());
            docNode->InsertEndChild(docTitle);
            //插入link节点
            XMLElement *docLink = myDoc->NewElement("link");
            docLink->SetText(link.c_str());
            docNode->InsertEndChild(docLink);
            //插入content节点
            XMLElement *docContent = myDoc->NewElement("content");
            docContent->SetText(content.c_str());
            docNode->InsertEndChild(docContent);
            length = base + std::to_string(docNum).size() + title.size() + link.size() + content.size();
            if (docNum == 4) {
                cout << content << "\n";
            }
            _offsetLib[docNum++] = std::make_pair(pos, length);
            pos += length;
            //找到下一个item节点
            itemNode = itemNode->NextSiblingElement("item");
        }
    }
    string pageLib = conf["PageLib"] + "/pageLib.xml";
    XMLError error = myDoc->SaveFile(pageLib.c_str());
    if (error == XML_SUCCESS) {
        cout << "PageLib create succeed!, docNum: " << docNum - 1
             << "\nthe file saved in: " << pageLib << "\n";
    } else {
        cerr << "PageLib create failed!\n";
        cerr << "error: " << error << "\n";
    }
    delete myDoc;
}*/
//创建网页库 使用ofstream构造xml文件
void PageLib::create() {
    map<string, string> &conf = Configuration::getInstance()->getConfigMap();
    string xmlDir = conf["WebCorpusDir"];
    auto &files = _dirScanner.getFiles(xmlDir);
    XMLDocument doc;//xml解析对象
    string pageLib = conf["PageLib"] + "/pageLib.xml";

    ofstream ofs(pageLib);
    // 预定义 XML 标签常量（C++17 起支持 constexpr string_view 更高效）
    constexpr const char *DOC_HEADER = "<doc>\n";
    constexpr const char *DOCID_OPEN = "    <docId>";
    constexpr const char *DOCID_CLOSE = "</docId>\n";
    constexpr const char *TITLE_OPEN = "    <title>";
    constexpr const char *TITLE_CLOSE = "</title>\n";
    constexpr const char *LINK_OPEN = "    <link>";
    constexpr const char *LINK_CLOSE = "</link>\n";
    constexpr const char *CONTENT_OPEN = "    <content>";
    constexpr const char *CONTENT_CLOSE = "</content>\n";
    constexpr const char *DOC_FOOTER = "</doc>\n\n";

    const size_t batchSz = 1024* 1024;//1M
    string pageContent;
    pageContent.reserve(batchSz);
    int docNum = 1;
    int base = 96;
    unsigned long long pos = 0;
    unsigned long long length = 0;
    for (auto &&file: files) {
        doc.LoadFile(file.c_str());
        if (doc.ErrorID() != 0) {
            cerr << "ErrorStr: " << doc.ErrorStr() << "\n";
            cerr << "ErrorName: " << doc.ErrorName() << "\n";
            cerr << "ErrorID: " << doc.ErrorID() << "\n";
            cerr << "Error loading file: " << file << "\n";
        }
        XMLElement *itemNode = doc.FirstChildElement("rss")
                ->FirstChildElement("channel")
                ->FirstChildElement("item");

        while (itemNode) {
            string title = removeHTMLTags(itemNode->FirstChildElement("title")->GetText());
            string link = itemNode->FirstChildElement("link")->GetText();
            string content;
            if (itemNode->FirstChildElement("description") != nullptr)
                content = removeHTMLTags(itemNode->FirstChildElement("description")->GetText());
            else//没有description节点
                content = "no description";

            pageContent.append(DOC_HEADER)
                    .append(DOCID_OPEN).append(std::to_string(docNum)).append(DOCID_CLOSE)
                    .append(TITLE_OPEN).append(title).append(TITLE_CLOSE)
                    .append(LINK_OPEN).append(link).append(LINK_CLOSE)
                    .append(CONTENT_OPEN).append(content).append(CONTENT_CLOSE)
                    .append(DOC_FOOTER);
            length = base + std::to_string(docNum).size() + title.size() + link.size() + content.size();

            _offsetLib[docNum++] = std::make_pair(pos, length);
            pos += length;
            //分批写入文件
            if (pageContent.size() >= batchSz) {
                ofs.write(pageContent.data(), pageContent.size());
                pageContent.clear();
//              pageContent.reserve(batchSz);由于字符串的容量已经是 1MB（或更大）不会执行任何实际的内存分配
            }
            //找到下一个item节点
            itemNode = itemNode->NextSiblingElement("item");
        }
    }
    if (!pageContent.empty()) {
        ofs.write(pageContent.data(), pageContent.size());
    }

    cout << "PageLib create succeed!, docNum: " << docNum - 1
         << "\nthe file saved in: " << pageLib << "\n";
    ofs.close();
}

PageLib::index &PageLib::getOffsetLib() {
    return _offsetLib;
}

void PageLib::store() {
}