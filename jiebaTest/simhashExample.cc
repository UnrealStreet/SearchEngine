#include <iostream>
#include "simhash/Simhasher.hpp"

using namespace std;
using namespace simhash;
//this define can avoid some logs which you don't need to care about.
/* #define LOGGER_LEVEL LL_WARN */

const char *const DICT_PATH = "../resource/dict/jieba.dict.utf8";
const char *const HMM_PATH = "../resource/dict/hmm_model.utf8";
const char *const IDF_PATH = "../resource/dict/idf.utf8";
const char *const STOP_WORD_PATH = "../resource/dict/stop_words.utf8";

int main(int argc, char **argv) {
    Simhasher simhasher(DICT_PATH, HMM_PATH, IDF_PATH, STOP_WORD_PATH);
    string s("我是蓝翔技工拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上总经理，出任CEO，走上人生巅峰。");
    size_t topN = 5;
    uint64_t u64 = 0;//将来存储simhash值
    vector<pair<string, double> > res;//将来存储关键词序列
    simhasher.extract(s, res, topN);//提取关键词
    simhasher.make(s, topN, u64);//计算simhash值
    cout << "文本：\"" << s << "\"" << endl;
    cout << "关键词序列是: " << res << endl;
    cout << "simhash值是: " << u64 << endl;

    const char *bin1 = "100010110110";
    const char *bin2 = "110001110011";
    uint64_t u1, u2;//将来存储simhash值
    u1 = Simhasher::binaryStringToUint64(bin1);//将二进制字符串转换为uint64_t类型
    u2 = Simhasher::binaryStringToUint64(bin2);//将二进制字符串转换为uint64_t类型

    cout << bin1 << "和" << bin2 << " simhash值的相等判断如下：" << endl;
    //isEqual函数用来判断两个simhash值是否相等
    cout << "海明距离阈值默认设置为3，则isEqual结果为：" << std::boolalpha << (Simhasher::isEqual(u1, u2)) << endl;
    cout << "海明距离阈值默认设置为5，则isEqual结果为：" << std::boolalpha << (Simhasher::isEqual(u1, u2, 5)) << endl;
    return EXIT_SUCCESS;
}
