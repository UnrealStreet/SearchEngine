#pragma once

#include "SplitTool.h"

namespace cppjieba {
    class Jieba;
}

class SplitToolCppJieBa : public SplitTool {
public:

    vector<string> cut(string txt) override;

    vector<string> cutForSearch(string txt) override;

    SplitToolCppJieBa(const SplitToolCppJieBa &) = delete;

    SplitToolCppJieBa &operator=(const SplitToolCppJieBa &) = delete;

    ~SplitToolCppJieBa() override;

    static SplitToolCppJieBa *getInstance();

private:
    SplitToolCppJieBa();

    static cppjieba::Jieba *_jieba;
};

