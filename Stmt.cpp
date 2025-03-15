#pragma once
#include "Stmt.h"
Stmt::Stmt(StmtType type, Exp* exp1, Exp* exp2, Exp* exp3, Exp* exp4)//最多接收四个参数，根据语句类型，只有部分参数有效
{
    this->type = type;
    if(exp1 != nullptr)exps.push_back(exp1);
    if(exp2 != nullptr)exps.push_back(exp2);
    if(exp3 != nullptr)exps.push_back(exp3);
    if(exp4 != nullptr)exps.push_back(exp4);
}


