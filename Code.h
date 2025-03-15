#pragma once
#include "CodeNode.h"
#include "Parser.h"
class Code {
public:
    //语法句法分析器
    Parser *parser;
    //代码链表
    CodeNode *root;
    Code(Parser *parser);
    void addCode(int lineNum, QString stmtInp);//向代码链表中添加一句代码
    void deleteCode(int lineNum);
    CodeNode *findCode(int lineNum);//根据行号找到对应的代码节点，若找不到返回nullptr(GOTO语句可能需要用)
    QString extractStr();//根据代码链表，提取出代码字符串，显示在代码显示框中
    QString getStrTree();//生成表达式树，显示在表达式树框中
};



