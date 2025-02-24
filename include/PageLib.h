#ifndef _PAGELIB_H
#define _PAGELIB_H

#include "DirScanner.h"
#include <unordered_map>

using std::pair;
using std::unordered_map;

class PageLib {
    using index = unordered_map<int, pair<int, int>>;
public:
    PageLib();

    void create();

    void store();

    index &getOffsetLib();
private:
private:
    DirScanner _dirScanner;
    vector<string> _pages;
    unordered_map<int, pair<int, int>> _offsetLib;;
};

#endif //_PAGELIB_H