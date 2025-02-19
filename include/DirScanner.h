#ifndef _DIRSCANNER_H
#define _DIRSCANNER_H
#include <string>
#include <vector>

using std::string;
using std::vector;

class DirScanner {
public:

    vector <string> &getFiles(const string &dirName);

//    void traverse(string dir);

private:
    vector <string> _files;
};

#endif //_DIRSCANNER_H