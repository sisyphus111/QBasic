#pragma once
#include "Code.h"
void Code::addCode(int lineNum, QString stmtInp)
{//向代码链表中加一句代码/修改某一句代码
    //先用parser解析stmt内容
    Stmt *stmt = parser->strToStmt(stmtInp);

    //再根据解析结果构造节点，加入链表
    if(root == nullptr)root = new CodeNode(lineNum, stmt);
    else{
        CodeNode *pl = root;
        CodeNode *pr = pl->next;
        if(pr == nullptr){//链表只有一个节点
            if(pl->lineNum < lineNum)pl->next = new CodeNode(lineNum, stmt);
            else if(pl->lineNum > lineNum)root = new CodeNode(lineNum, stmt, pl);
            else if(pl->lineNum == lineNum)pl->setStmt(stmt);
        }
        else{//链表有至少两个节点
            //corner case 1: lineNum < root->lineNum
            if(lineNum < root->lineNum){
                root = new CodeNode(lineNum, stmt, root);
                return;
            }
            else{
                while(pr != nullptr){
                    //寻找合适的位置，并进行处理
                    if(pl->lineNum < lineNum && lineNum < pr->lineNum){
                        pl->next = new CodeNode(lineNum, stmt, pr);
                        return;
                    }
                    else if(lineNum == pl->lineNum){
                        pl->setStmt(stmt);
                        return;
                    }
                    pl = pl->next;
                    pr = pr->next;
                }
                //循环结束后，pl指向链表最后一个节点
                //corner case 2: lineNum >= tail->lineNum
                if(lineNum == pl->lineNum){
                    pl->setStmt(stmt);
                    return;
                }
                else if(lineNum > pl->lineNum){
                    pl->next = new CodeNode(lineNum, stmt);
                    return;
                }
            }
        }
    }
}


void Code::deleteCode(int lineNum)
{
    if(root == nullptr)return;
    //根据行号删除某行代码
    if(lineNum == root->lineNum){//要删的是第一行代码
        CodeNode *tmp = root;
        root = root->next;
        delete tmp;
    }
    else{
        CodeNode *pl = root;
        CodeNode *pr = pl->next;
        while(pr != nullptr){
            if(pr->lineNum == lineNum){
                pl->next = pr->next;
                delete pr;
            }
            pl = pl->next;
            pr = pr->next;
        }
    }
    return;
}

CodeNode *Code::findCode(int lineNum)//根据行号找到对应的代码节点，若找不到返回nullptr
{
    CodeNode *p = root;
    while(p != nullptr){
        if(p->lineNum == lineNum)return p;
        p = p->next;
    }
    return p;
}

QString Code::extractStr()//根据代码链表，提取出代码字符串，显示在代码显示框中
{
    QString result = "";
    CodeNode *p = root;
    while(p != nullptr) {//遍历链表，一行一行生成字符串
        result += (QString::number(p->lineNum)) + " " + parser->extractStmtStr(p->stmt) + "\n";
        p = p->next;
    }
    return result;
}

Code::Code(Parser *parser)
{
    this->parser = parser;
    root = nullptr;
}

QString Code::getStrTree()
{
    QString result = "";
    CodeNode *p = root;
    while(p != nullptr){
        QString lineNumStr = QString::number(p->lineNum);
        QString typeStr;
        if(p->stmt->type == REM)typeStr = " REM\n";
        else if(p->stmt->type == LET)typeStr = " LET =\n";
        else if(p->stmt->type == PRINT)typeStr = " PRINT\n";
        else if(p->stmt->type == INPUT)typeStr = " INPUT\n";
        else if(p->stmt->type == GOTO)typeStr = " GOTO\n";
        else if(p->stmt->type == IF)typeStr = " IF THEN\n";
        else if(p->stmt->type == END)typeStr = " END\n";
        else typeStr = " INVALID\n";
        QString contentStr = parser->extractStmtTreeStr(p->stmt, 4);//每层缩进4格，生成stmt内容的字符串
        result += lineNumStr + typeStr + contentStr;
        p = p->next;
    }
    return result;
}