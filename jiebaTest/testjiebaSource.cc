#include <iostream>
#include "cppjieba/Jieba.hpp"
#include <algorithm>

using namespace std;

const char *const DICT_PATH = "../resource/dict/jieba.dict.utf8";
const char *const HMM_PATH = "../resource/dict/hmm_model.utf8";
const char *const USER_DICT_PATH = "../resource/dict/user.dict.utf8";
const char *const IDF_PATH = "../resource/dict/idf.utf8";
const char *const STOP_WORD_PATH = "../resource/dict/stop_words.utf8";

size_t getByteNum_UTF8(const char byte) {
    int byteNum = 0;
    for (size_t i = 0; i < 6; ++i) {
        if (byte & (1 << (7 - i)))
            ++byteNum;
        else
            break;
    }
    return byteNum == 0 ? 1 : byteNum;
}

int main() {
    cppjieba::Jieba jieba(DICT_PATH,
                          HMM_PATH,
                          USER_DICT_PATH,
                          IDF_PATH,
                          STOP_WORD_PATH);
    vector<string> words;
    string sentence = "文学、艺术理论的建设和发展，应该密\n切关注文学、艺术的实际存在状态及其已然或可能发生的变化。"
                      " 这样一个浅近的道理大约谁都不会怀疑。然而，当下正在我们身边发生的艺术世界总体格局的巨大变化却又常常为我们视而不见。"
                      " 这不能不带来诸多理论上的遗憾。";
//    string sentence = "我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，走上人生巅峰。\n";
    /* string sentence = R"(文学、艺术理论的建设和发展，应该密切关注文学、艺术的实际存在状态及其已然或可能发生的变化。 */
/* 这样一个浅近的道理大约谁都不会怀疑。然而，当下正在我们身边发生的艺术世界总体格局的巨大变化却又常常为我们视而不见。 */
/* 这不能不带来诸多理论上的遗憾。)"; */
    sentence.erase(std::remove(sentence.begin(), sentence.end(), '\n'), sentence.end());
    cout << sentence << endl;
    jieba.Cut(sentence, words, true);
    for (auto &word: words) {
        if (getByteNum_UTF8(word[0]) > 1)
            cout << word << "/";
    }
    cout << endl;
    cout << limonp::Join(words.begin(), words.end(), "/") << endl;
    //limonp::Join的三参数版本，第三个参数是分隔符
//    cout << limonp::Join(words.begin(),words.end(),"/") << endl;;
    //limonp::Join的四参数版本，第三个参数是要保存分词结果的字符串，第四个参数是分隔符
//    string res;
//    limonp::Join(words.begin(), words.end(),res,"/");
}

