#ifndef _SPLITTOOLCPPJIEBA_H
#define _SPLITTOOLCPPJIEBA_H

#include "SplitTool.h"

namespace cppjieba {
    class Jieba;
}

class SplitToolCppJieBa : public SplitTool {
public:

    vector<string> cut(string txt) override;

    vector<string> cutForSearch(string txt) override;

    ~SplitToolCppJieBa();
private:
    static cppjieba::Jieba *_jieba;
};

#endif //_SPLITTOOLCPPJIEBA_H