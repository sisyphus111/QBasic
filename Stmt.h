#pragma once
#include <vector>
#include "Exp.h"

//语句类型：REM、LET、PRINT、INPUT、GOTO、IF、END
enum StmtType{ REM, LET, PRINT, INPUT, GOTO, IF, END, STMT_INVALID, STMT_LINENUM_OUTOFRANGE};

class Stmt {
public:
    StmtType type;
    //对不同语句，exps向量的排列方式不同
    //赋值LET语句：LET var = exp，则exps[0]为var，exps[1]为exp
    //IF THEN语句，IF m > max THEN n ，exps[0]为m，exps[1]为>，exps[2]为max，exps[3]为n
    //GOTO语句：GOTO n，则exps[0]为n
    //PRINT语句：PRINT exp，则exps[0]为exp
    //INPUT语句：INPUT var，则exps[0]为var
    //REM语句：REM后面的内容为注释，exps[0]为注释内容
    std::vector<Exp*> exps;
    Stmt(StmtType type, Exp* exp1 = nullptr, Exp* exp2 = nullptr, Exp* exp3 = nullptr, Exp* exp4 = nullptr);//最多接收四个参数，根据语句类型，只有部分参数有效
};


