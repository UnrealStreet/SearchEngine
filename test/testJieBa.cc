#include <iostream>
#include <algorithm>
#include "SplitToolCppJieba.h"
using namespace std;

const char* const DICT_PATH = "../resource/dict/jieba.dict.utf8";
const char* const HMM_PATH = "../resource/dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../resource/dict/user.dict.utf8";
const char* const IDF_PATH = "../resource/dict/idf.utf8";
const char* const STOP_WORD_PATH = "../resource/dict/stop_words.utf8";


int main(){
    SplitToolCppJieBa *jieba = SplitToolCppJieBa::getInstance();
    vector<string> words;
    string sentence = "文学、艺术理论的建设和发展，应该密切关注文学、艺术的实际存在状态及其已然或可能发生的变化。\n"
                      "这样一个浅近的道理大约谁都不会怀疑。\n然而，当下正在我们身边发生的艺术世界总体格局的巨大变化却又常常为我们视而不见。\n"
                      "这不能不带来诸多理论上的遗憾。";
    /* string sentence = R"(文学、艺术理论的建设和发展，应该密切关注文学、艺术的实际存在状态及其已然或可能发生的变化。 */
/* 这样一个浅近的道理大约谁都不会怀疑。然而，当下正在我们身边发生的艺术世界总体格局的巨大变化却又常常为我们视而不见。 */
/* 这不能不带来诸多理论上的遗憾。)"; */
    cout << sentence << endl;
    sentence.erase(std::remove(sentence.begin(), sentence.end(), '\n'), sentence.end());
    words = jieba->cut(sentence);
    for(auto &word : words) {
        cout << word << "/";
    }
    //limonp::Join的三参数版本，第三个参数是分隔符
//    cout << limonp::Join(words.begin(),words.end(),"/") << endl;;
}

