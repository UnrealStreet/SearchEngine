#ifndef _SPLITTOOL_H
#define _SPLITTOOL_H

#include <string>
#include <vector>

using std::string;
using std::vector;

class SplitTool {
public:
    virtual ~SplitTool();

    virtual vector<string> cut(string txt) = 0;

    virtual vector<string> cutForSearch(string txt) = 0;
};

#endif //_SPLITTOOL_H