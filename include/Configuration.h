#ifndef _CONFIGURATION_H
#define _CONFIGURATION_H

#include <string>
#include <map>
#include <set>
#include <vector>

using std::string;
using std::map;
using std::set;
using std::vector;

class Configuration {
public:
    //单例模式
    static Configuration *getInstance();
    //获取配置文件
    map<string, string> &getConfigMap();
    //获取停用词
    set<string> getStopWords();
    //获取字典文件
    vector<string> getDictFiles();

    Configuration(const Configuration &) = delete;

    Configuration &operator=(const Configuration &) = delete;

private:
    Configuration();

private:
    string _configFilePath;
    map<string, string> _configs;
};

#endif //_CONFIGURATION_H