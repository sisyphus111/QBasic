#pragma once
#include "Stmt.h"
#include "Parser.h"

class CodeNode {
public:
    int lineNum;
    Stmt *stmt;
    CodeNode *next;
    CodeNode(int lineNum, Stmt *stmt, CodeNode *next = nullptr);
    void setStmt(Stmt* stmt);
};


