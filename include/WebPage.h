#pragma once
#include <string>
#include "tinyxml2.h"

using std::string;
class WebPage {
public:
    void ParseDoc(const string &doc);

    void clear();

    int _docId;
    string _docTitle;
    string _docLink;
    string _docContent;
private:
    tinyxml2::XMLDocument _xml;
};
