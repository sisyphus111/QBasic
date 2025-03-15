#pragma once
#include "ExpNode.h"

enum EXP_TYPE { num, op, str, var };
//var指纯的变量，其他含变量/不含变量表达式都输入num
class Exp {
public:
    //该表达式的类型：是数值/运算符/字符串/变量
    EXP_TYPE type;
    ExpNode *root;
    Exp(EXP_TYPE type, ExpNode *root);
};



