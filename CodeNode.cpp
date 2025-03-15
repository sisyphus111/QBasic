#include "CodeNode.h"
CodeNode::CodeNode(int lineNum, Stmt *stmt, CodeNode *next)
{
    this->lineNum = lineNum;
    this->next = next;
    this->stmt = stmt;
}


void CodeNode::setStmt(Stmt *stmt)
{//修改某一行代码时，通过此函数修改
    if(this->stmt)delete this->stmt;
    this->stmt = stmt;
}