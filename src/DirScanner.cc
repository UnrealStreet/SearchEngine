#include "DirScanner.h"
#include <dirent.h>
#include <cstring>
#include <iostream>

using std::cerr;
using std::endl;

vector<string> &DirScanner::getFiles(const string &_dirName) {
    DIR *dir = opendir(_dirName.c_str());
    if (dir == nullptr) {
        cerr << "opendir error" << endl;
        return _files;
    }
    struct dirent *ptr;
    while ((ptr = readdir(dir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
            _files.push_back(_dirName + "/" + ptr->d_name);
        }
    }
    closedir(dir);
    return _files;
}

//void DirScanner::traverse(string dir) {
//    return;
//}