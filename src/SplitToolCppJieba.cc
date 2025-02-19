#include "SplitToolCppJieba.h"
#include "cppjieba/Jieba.hpp"

cppjieba::Jieba *SplitToolCppJieBa::_jieba = new cppjieba::Jieba("../resource/dict/jieba.dict.utf8",
                                                               "../resource/dict/hmm_model.utf8",
                                                               "../resource/dict/user.dict.utf8",
                                                               "../resource/dict/idf.utf8",
                                                               "../resource/dict/stop_words.utf8");

SplitToolCppJieBa::~SplitToolCppJieBa() {
    delete _jieba;
}

vector<string> SplitToolCppJieBa::cut(string txt) {
    vector<string> words;
    _jieba->Cut(txt, words, true);
//    std::cout << limonp::Join(words.begin(),words.end(),"/") << std::endl;
    return std::move(words);
}

vector<string> SplitToolCppJieBa::cutForSearch(string txt) {
    vector<string> words;
    _jieba->CutForSearch(txt, words, true);
//    std::cout << limonp::Join(words.begin(),words.end(),"/") << std::endl;
    return std::move(words);
}
