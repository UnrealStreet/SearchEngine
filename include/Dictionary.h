#ifndef _DICTIONARY_H
#define _DICTIONARY_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <mutex>

using std::string;
using std::vector;
using std::pair;
using std::set;
using std::map;
using std::mutex;

class Dictionary {
    class AutoRelease {
    public:
        AutoRelease();

        ~AutoRelease();
    };

public:
    Dictionary(const Dictionary &) = delete;

    Dictionary &operator=(const Dictionary &) = delete;

    vector<string> doQuery(const string &key);

    void queryIndex();

    static Dictionary *getInstance();

    void initDict(const string &dictPath);

    void initIndex(const string &indexPath);

    vector<pair<string, int>> &getDict();

    map<string, set<int>> &getIndex();

    static void destroy();

private:
    Dictionary();

    //如果想用智能指针接管内存管理，需要将析构函数设为public
    //并在析构函数中设置_pInstance = nullptr，防止double free
    ~Dictionary();


private:
    vector<pair<string, int>> _dict;
    map<string, set<int>> _index;
    static mutex _mutex;
    static Dictionary *_pInstance;
    static AutoRelease _autoRelease;
};

#endif //_DICTIONARY_H