#pragma once
#include <unordered_map>
#include <string>
#include <QString>
class Status {
public:
    Status();
    //添加变量（键值对）
    void addVar(const std::string &varName, int value);
    //删除变量
    void deleteVar(const std::string &varName);
    //判断是否存在某变量
    bool isVarExist(const std::string &varName) const;
    //修改变量值
    void modifyVar(const std::string &varName, int value);
    //查找变量
    int findVar(const std::string &varName) const;
    //清空所有存储的键值对
    void clearVars();
    QString getVarStr();//输出存储变量的信息，显示在变量展示框中
private:
    std::unordered_map<std::string, int> variables;
};
