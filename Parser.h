#pragma once
#include <QString>
#include "Stmt.h"

//输入内容的类型：带行号代码、不带行号代码、命令
enum InputType{ INDEXED, UNINDEXED, CMD, INP_INVALID, INPUT_LINENUM_OUTOFRANGE };

//操作符
enum Ops{ ADD, SUB, MUL, DIV, MOD, POW, LPARA, RPARA, INVALID};

class Parser {
public:
    Parser();
    //判断输入内容的类型
    InputType tellInpType(const QString input);
    //判断语句类型
    StmtType tellStmtType(const QString stmt);
    //判断一个表达式是否合法（仅从形式上判断）
    bool tellExpValidity(const std::string exp);
    //判断是不是合法变量名
    bool tellVarValidity(const std::string var);
    //去掉操作符周围的空格--tellExpValidity的辅助函数
    std::string removeSpacesAroundOperators(const std::string& exp);
    //判断表达式中的括号匹配情况--tellExpValidity的辅助函数
    std::vector<std::pair<int, int>> findParasPairs(const std::string& exp);
    //将带行号代码的输入分为行号和代码两部分
    std::pair<int, QString> splitIndexedInput(const QString inp);
    //从Stmt类型提取出字符串
    QString extractStmtStr(Stmt *stmt);
    //从Exp类型提取出字符串
    QString extractExpStr(Exp *exp);
    //将表达式树和字符串互相转换的两个函数
    ExpNode *strToExpTree(QString exp);
    QString expTreeToStr(ExpNode *node);
    //将Stmt类和字符串互相转换
    Stmt *strToStmt(QString inp);
    QString stmtToStr(Stmt *stmt);
    //判断一个字符串中的某处的操作符种类（若不是，则返回INVALID）
    Ops tellOperator(size_t i, const std::string &str);
    //对两个运算符判断优先级
    int tellPrecedence(Ops op);
    int tellPrecedence(QString op);
    int tellPrecedence(std::string op);
    QString getOpStr(Ops op);//根据运算符获取其字符串形式
    QString extractStmtTreeStr(Stmt *stmt, int spaceNum);//提取出stmt的树形字符串
    QString extractExpTreeStr(ExpNode *node, int spaceNum);//提取出exp的树形字符串
};



