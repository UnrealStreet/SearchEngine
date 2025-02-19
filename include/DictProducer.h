#ifndef _DICTPRODUCER_H
#define _DICTPRODUCER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>

using std::string;
using std::vector;
using std::pair;
using std::set;
using std::unordered_map;

class SplitTool;

class DictProducer {
public:

    DictProducer(SplitTool* tool);

    void buildEnDict();

    void buildCnDict();

    void buildIndex();

    void storeDict(const string &filepath);

    void storeIndex(const string &filepath);

private:
    void readDict();

    size_t getByteNum_UTF8(const char byte);

    void getFiles(int fileNum);

    string dealEnWord(string &word);

private:
    vector<string> _files;
    vector<pair<string, int>> _dict;
    unordered_map<string, set<int>> _index;
    SplitTool *_cuttor;
    set<string> _stopWords;
};

#endif //_DICTPRODUCER_H