#pragma once
#include <QString>

enum EXP_NODE_TYPE { exp_var, exp_num, exp_str, exp_op };
class ExpNode {
public:
    //该节点储存的数据类型：
    //变量——exp_var
    //数字——exp_num
    //操作符——exp_op
    //字符串--exp_str
    EXP_NODE_TYPE type;
    //当前节点储存的数据
    //如果是变量，存储变量名
    //如果是数字，存储数字
    //如果是操作符，存储操作符
    QString data;
    //表达式树的左右子树
    ExpNode* left;
    ExpNode* right;
    //构造函数
    ExpNode(EXP_NODE_TYPE type, QString data, ExpNode* left = nullptr, ExpNode* right = nullptr);
    ExpNode(std::string data);//接收一个字符串（变量或数字），构造无儿子ExpNode类对象
};



