#pragma once
#include "Code.h"
#include "Status.h"
#include <set>
#include <QEventLoop>

class Program : public QObject{
    Q_OBJECT
public:
    std::set<int> bPoints;//储存打了断点的行号
    CodeNode *codeCounter;//指向下一句待执行代码的指针（仅在调试模式中使用，平时置为nullptr）
    Code *code;//代码
    Status *status;//状态
    Parser *parser;//语法句法分析器
    QEventLoop loop;
    void run();//以非调试模式运行程序
    void debugStart();//以调试模式开始运行程序
    void debugResume();//从暂停状态下继续运行
    void addCode(int lineNum, QString stmt);//向代码链表中加一句代码
    void deleteCode(int lineNum);//根据行数删除某行代码
    QString extractStr();//提取出代码字符串，显示在代码显示框中
    QString getStrTree();//生成表达式树，显示在表达式树框中
    void addBPoint(int lineNum);//添加断点
    void deleteBPoint(int lineNum);//删除断点
    int calculateExp(ExpNode *root);//输入表达式树，计算表达式的值（若有变量则从status里面找）
    QString getBPointStr();//生成断点位置字符串（显示在断点框中）
    void exitDebug();//重置所有与调试有关的数据
    Program(Parser *parser, Status *status);
signals:
    void printSignal(QString str);//发送给MainWindow类，更新输出框
    void waitInput(QString var);//发送给MainWindow类，用来等待输入变量的值
};



