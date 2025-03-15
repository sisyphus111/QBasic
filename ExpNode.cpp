#pragma once
#include "ExpNode.h"
#include <sstream>
//构造函数
ExpNode::ExpNode(EXP_NODE_TYPE type, const QString data, ExpNode* left, ExpNode* right)
{
    this->type = type;
    this->data = data;
    this->left = left;
    this->right = right;
}

ExpNode::ExpNode(std::string data)
{//接收一个字符串（只可能是单个的数字或单个的变量），构造无儿子的ExpNode类对象
    std::stringstream ss(data);
    int num;
    if(ss >> num){
        //是个数
        this->type = exp_num;
        this->data = QString::fromStdString(data);
        this->left = nullptr;
        this->right = nullptr;
    }
    else{
        //是单个变量
        this->type = exp_var;
        this->data = QString::fromStdString(data);
        this->left = nullptr;
        this->right = nullptr;
    }
}