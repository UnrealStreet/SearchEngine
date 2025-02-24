#include "WebPage.h"

#include <iostream>

using std::cerr;

//tinyxml2::XMLDocument WebPage::_xml;

void WebPage::ParseDoc(const string &doc) {
    clear();//清空原有数据
    using namespace ::tinyxml2;
    _xml.Parse(doc.c_str());
    if (_xml.ErrorID() != XML_SUCCESS) {
        cerr << "ErrorStr: " << _xml.ErrorStr() << "\n";
    }
    XMLElement *docNode = _xml.FirstChildElement("doc");
    _docId = atoi(docNode->FirstChildElement("docId")->GetText());
    _docTitle = docNode->FirstChildElement("title")->GetText();
    _docLink = docNode->FirstChildElement("link")->GetText();
    XMLElement *contentNode = docNode->FirstChildElement("content");
    if (contentNode->GetText())
        _docContent = contentNode->GetText();
    else
        _docContent = "no content";
}

void WebPage::clear() {
    _docId = 0;
    _docTitle.clear();
    _docLink.clear();
    _docContent.clear();
}