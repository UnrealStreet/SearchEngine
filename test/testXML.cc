/**
  ******************************************************************************
  * @file           : testXML.cc
  * @author         : GOD婴宁
  * @brief          : None
  * @attention      : None
  * @date           : 2025/2/11
  ******************************************************************************
  */
#include <iostream>
#include <string>
#include <regex>
#include "PageLib.h"
#include <fstream>
#include <sstream>

using namespace std;
//using namespace tinyxml2;

//const string xmlPath = "../resource/coolshell.xml";

//string removeHTMLTags(const string &input) {
//    // 定义一个正则表达式，匹配 HTML 标签
//    regex htmlTagRegex("<[^>]*>");
//    // 使用 regex_replace 将标签替换为空字符串
//    return regex_replace(input, htmlTagRegex, "");
//}

/*void test() {
    XMLDocument doc;
    doc.LoadFile(xmlPath.c_str());

    if (doc.ErrorID() != 0) {
        cerr << "ErrorStr: " << doc.ErrorStr() << endl;
        cerr << "ErrorName: " << doc.ErrorName() << endl;
        cerr << "ErrorID: " << doc.ErrorID() << endl;
    }

    XMLElement *itemNode = doc.FirstChildElement("rss")
            ->FirstChildElement("channel")
            ->FirstChildElement("item");
    while (itemNode) {
        string title = itemNode->FirstChildElement("title")->GetText();
        string link = itemNode->FirstChildElement("link")->GetText();
        string description = removeHTMLTags(itemNode->FirstChildElement("description")->GetText());
        string content = removeHTMLTags(itemNode->FirstChildElement("content:encoded")->GetText());
        cout << "title: " << title << endl;
        cout << "link: " << link << endl;
        cout << "description: " << description << endl;
        cout << "content: " << content << endl;
        itemNode = itemNode->NextSiblingElement("item");
    }
}
//测试错误的xml文件edu.xml错误
void testError(){
    XMLDocument doc;
    doc.LoadFile("../resource/WebCorpus/sh.xml");
    if (doc.ErrorID() != 0) {
        cerr << "ErrorStr: " << doc.ErrorStr() << endl;
        cerr << "ErrorName: " << doc.ErrorName() << endl;
        cerr << "ErrorID: " << doc.ErrorID() << endl;
    }
    XMLElement *itemNode = doc.FirstChildElement("rss")
            ->FirstChildElement("channel")
            ->FirstChildElement("item");
    while (itemNode) {
        string title = itemNode->FirstChildElement("title")->GetText();
        cout << "title: " << title << endl;
        string link = itemNode->FirstChildElement("link")->GetText();
        cout << "link: " << link << endl;
        string description = removeHTMLTags(itemNode->FirstChildElement("description")->GetText());
        cout << "description: " << description << endl;
        itemNode = itemNode->NextSiblingElement("item");
    }
}

void succeed(){
    auto &map = Configuration::getInstance()->getConfigMap();
    string xmlDir = map["WebCorpusDir"];
    DirScanner scanner;
    auto &files = scanner.getFiles(xmlDir);
    for (auto &&file: files) {
        cout << file << endl;
    }
    cout << "----------------------\n";
    XMLDocument doc;
    for (auto &&file: files) {
        doc.LoadFile(file.c_str());
        if (doc.ErrorID() != 0) {
            cerr << "ErrorStr: " << doc.ErrorStr() << "\n";
            cerr << "ErrorName: " << doc.ErrorName() << "\n";
            cerr << "ErrorID: " << doc.ErrorID() << "\n";
        }
        cout << "file is open succeed: " << file << endl;
        XMLElement *itemNode = doc.FirstChildElement("rss")
                ->FirstChildElement("channel")
                ->FirstChildElement("item");
        while (itemNode) {
            string title = itemNode->FirstChildElement("title")->GetText();
            cout << "title: " << title << endl;
            string link = itemNode->FirstChildElement("link")->GetText();
            cout << "link: " << link << endl;
            string description;
            if (itemNode->FirstChildElement("description") != nullptr)
                description = removeHTMLTags(itemNode->FirstChildElement("description")->GetText());
            else
                description = "no description";
            cout << "description: " << description << endl;
            itemNode = itemNode->NextSiblingElement("item");
        }
    }
}*/
int main() {
    PageLib pageLib;
    pageLib.create();
    auto & offsetLib = pageLib.getOffsetLib();
    ostringstream oss;
    for (auto &&elem: offsetLib) {
        oss << elem.first << " " << elem.second.first << " " << elem.second.second << "\n";
    }
    ofstream ofs("../data/offsetLib.dat");
    ofs << oss.str();
    ofs.close();
    cout << "offsetLib store success\n";
    return 0;
}
