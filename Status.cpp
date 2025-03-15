#pragma once
#include "Status.h"

Status::Status()
{
    variables.clear();
}

void Status::addVar(const std::string &varName, int value)
{//添加新变量
    variables[varName] = value;
}

void Status::deleteVar(const std::string &varName)
{//删除某个变量
    variables.erase(varName);
}

void Status::modifyVar(const std::string &varName, int value)
{//修改某变量的值
    variables[varName] = value;
}

bool Status::isVarExist(const std::string &varName) const
{//判断变量是否存在
    auto it = variables.find(varName);
    if (it != variables.end()) {
        return true;
    }
    else return false;
}

int Status::findVar(const std::string &varName) const
{//根据变量名查找变量的值(需要先确定变量存在)
    auto it = variables.find(varName);
    if (it != variables.end()) {
        return it->second;
    }
}

QString Status::getVarStr()
{//生成展示存储的变量信息的字符串——显示到变量展示框中
    QString result = "";
    for (const auto &pair : variables) {
        result += QString::fromStdString(pair.first) + " = " + QString::number(pair.second)  + "\n";
    }
    return result;
}

void Status::clearVars()
{//清空所有存储的键值对
    variables.clear();
}