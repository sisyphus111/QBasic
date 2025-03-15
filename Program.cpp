#pragma once
#include "Program.h"

//注：对代码进行的添加、删改操作只能在非调试模式下进行！
//"code edits are disabled in debug mode"
void Program::run()
{//以非调试模式运行程序
    //从代码链表的头节点开始执行（下一句待执行代码节点的地址存放在codeCounter中）
    this->codeCounter = code->root;
    while(this->codeCounter != nullptr){






        //执行codeCounter指向的代码节点，并在执行完之后更新codeCounter的值（GOTO或IF THEN语句有特殊处理）
        if(codeCounter->stmt->type == REM)codeCounter = codeCounter->next;
        else if(codeCounter->stmt->type == LET){
            //LET语句
            //先计算等号右边的表达式的值
            int value = this->calculateExp(codeCounter->stmt->exps[1]->root);
            //将值赋给变量
            if(status->isVarExist(codeCounter->stmt->exps[0]->root->data.toStdString())){
                status->modifyVar(codeCounter->stmt->exps[0]->root->data.toStdString(), value);
            }
            else{
                status->addVar(codeCounter->stmt->exps[0]->root->data.toStdString(), value);
            }
            codeCounter = codeCounter->next;
        }
        else if(codeCounter->stmt->type == PRINT){
            int value = this->calculateExp(codeCounter->stmt->exps[0]->root);
            //输出value
            //发送信号
            emit printSignal(QString::number(value));
            codeCounter = codeCounter->next;
        }
        else if(codeCounter->stmt->type == INPUT){
            //在输入变量的值前阻塞进程
            emit waitInput(codeCounter->stmt->exps[0]->root->data);
            //等待输入
            loop.exec();
            codeCounter = codeCounter->next;
        }
        else if(codeCounter->stmt->type == GOTO){
            int lineNum = this->calculateExp(codeCounter->stmt->exps[0]->root);
            CodeNode *tmp = code->findCode(lineNum);
            if(tmp == nullptr){
                //跳到不存在的行号，报错
                //待完成

            }
            else{
                codeCounter = tmp;
            }
        }
        else if(codeCounter->stmt->type == IF){
            bool cond = false;
            int lval = this->calculateExp(codeCounter->stmt->exps[0]->root);
            int rval = this->calculateExp(codeCounter->stmt->exps[2]->root);
            if(codeCounter->stmt->exps[1]->root->data == "<")cond = (lval < rval);
            else if(codeCounter->stmt->exps[1]->root->data == "=")cond = (lval == rval);
            else if(codeCounter->stmt->exps[1]->root->data == ">")cond = (lval > rval);
            if(cond){
                int lineNum = this->calculateExp(codeCounter->stmt->exps[3]->root);
                CodeNode *tmp = code->findCode(lineNum);
                if(tmp == nullptr){
                    //跳到不存在的行号，报错
                    //待完成


                }
                else{
                    codeCounter = tmp;
                }
            }
            else{
                codeCounter = codeCounter->next;
            }
        }
        else if(codeCounter->stmt->type == END)break;









    }
    return;
}


void Program::debugStart()
{//开始调试程序
    //从代码链表的头节点开始执行（下一句待执行代码节点的地址存放在codeCounter中）
    this->codeCounter = code->root;
    //若第一行就是断点，则直接返回
    if(code->root == nullptr || bPoints.count(code->root->lineNum) > 0)return;
    if(codeCounter == nullptr)return;//根本没有代码，不需要调试
    do{





        //执行codeCounter指向的代码节点，并在执行完之后更新codeCounter的值（GOTO或IF THEN语句有特殊处理）
        if(codeCounter->stmt->type == REM)codeCounter = codeCounter->next;
        else if(codeCounter->stmt->type == LET){
            //LET语句
            //先计算等号右边的表达式的值
            int value = this->calculateExp(codeCounter->stmt->exps[1]->root);
            //将值赋给变量
            if(status->isVarExist(codeCounter->stmt->exps[0]->root->data.toStdString())){
                status->modifyVar(codeCounter->stmt->exps[0]->root->data.toStdString(), value);
            }
            else{
                status->addVar(codeCounter->stmt->exps[0]->root->data.toStdString(), value);
            }
            codeCounter = codeCounter->next;
        }
        else if(codeCounter->stmt->type == PRINT){
            int value = this->calculateExp(codeCounter->stmt->exps[0]->root);
            //输出value
            //发送信号
            emit printSignal(QString::number(value));
            codeCounter = codeCounter->next;
        }
        else if(codeCounter->stmt->type == INPUT){
            //在输入变量的值前阻塞进程
            emit waitInput(codeCounter->stmt->exps[0]->root->data);
            //等待输入
            loop.exec();
            codeCounter = codeCounter->next;
        }
        else if(codeCounter->stmt->type == GOTO){
            int lineNum = this->calculateExp(codeCounter->stmt->exps[0]->root);
            CodeNode *tmp = code->findCode(lineNum);
            if(tmp == nullptr){
                //跳到不存在的行号，报错
                //待完成

            }
            else{
                codeCounter = tmp;
            }
        }
        else if(codeCounter->stmt->type == IF){
            bool cond = false;
            int lval = this->calculateExp(codeCounter->stmt->exps[0]->root);
            int rval = this->calculateExp(codeCounter->stmt->exps[2]->root);
            if(codeCounter->stmt->exps[1]->root->data == "<")cond = (lval < rval);
            else if(codeCounter->stmt->exps[1]->root->data == "=")cond = (lval == rval);
            else if(codeCounter->stmt->exps[1]->root->data == ">")cond = (lval > rval);
            if(cond){
                int lineNum = this->calculateExp(codeCounter->stmt->exps[3]->root);
                CodeNode *tmp = code->findCode(lineNum);
                if(tmp == nullptr){
                    //跳到不存在的行号，报错
                    //待完成


                }
                else{
                    codeCounter = tmp;
                }
            }
            else{
                codeCounter = codeCounter->next;
            }
        }
        else if(codeCounter->stmt->type == END)break;











    }while(this->codeCounter != nullptr && bPoints.count(codeCounter->lineNum) == 0);//执行到最后，或者遇到断点
    return;
}

void Program::debugResume()
{//从暂停状态继续调试，此时codeCounter理论上应该指向某个断点
    if(codeCounter == nullptr)return;//已经结束了，不需要调试
    do{





        //执行codeCounter指向的代码节点，并在执行完之后更新codeCounter的值（GOTO或IF THEN语句有特殊处理）
        if(codeCounter->stmt->type == REM)codeCounter = codeCounter->next;
        else if(codeCounter->stmt->type == LET){
            //LET语句
            //先计算等号右边的表达式的值
            int value = this->calculateExp(codeCounter->stmt->exps[1]->root);
            //将值赋给变量
            if(status->isVarExist(codeCounter->stmt->exps[0]->root->data.toStdString())){
                status->modifyVar(codeCounter->stmt->exps[0]->root->data.toStdString(), value);
            }
            else{
                status->addVar(codeCounter->stmt->exps[0]->root->data.toStdString(), value);
            }
            codeCounter = codeCounter->next;
        }
        else if(codeCounter->stmt->type == PRINT){
            int value = this->calculateExp(codeCounter->stmt->exps[0]->root);
            //输出value
            //发送信号
            emit printSignal(QString::number(value));
            codeCounter = codeCounter->next;
        }
        else if(codeCounter->stmt->type == INPUT){
            //在输入变量的值前阻塞进程
            emit waitInput(codeCounter->stmt->exps[0]->root->data);
            //等待输入
            loop.exec();
            codeCounter = codeCounter->next;
        }
        else if(codeCounter->stmt->type == GOTO){
            int lineNum = this->calculateExp(codeCounter->stmt->exps[0]->root);
            CodeNode *tmp = code->findCode(lineNum);
            if(tmp == nullptr){
                //跳到不存在的行号，报错
                //待完成

            }
            else{
                codeCounter = tmp;
            }
        }
        else if(codeCounter->stmt->type == IF){
            bool cond = false;
            int lval = this->calculateExp(codeCounter->stmt->exps[0]->root);
            int rval = this->calculateExp(codeCounter->stmt->exps[2]->root);
            if(codeCounter->stmt->exps[1]->root->data == "<")cond = (lval < rval);
            else if(codeCounter->stmt->exps[1]->root->data == "=")cond = (lval == rval);
            else if(codeCounter->stmt->exps[1]->root->data == ">")cond = (lval > rval);
            if(cond){
                int lineNum = this->calculateExp(codeCounter->stmt->exps[3]->root);
                CodeNode *tmp = code->findCode(lineNum);
                if(tmp == nullptr){
                    //跳到不存在的行号，报错
                    //待完成


                }
                else{
                    codeCounter = tmp;
                }
            }
            else{
                codeCounter = codeCounter->next;
            }
        }
        else if(codeCounter->stmt->type == END)break;









    }while(this->codeCounter != nullptr && bPoints.count(codeCounter->lineNum) == 0);//执行到最后，或者遇到断点
    return;
}


void Program::addCode(int lineNum, QString stmt)//向代码链表中加一句代码
{
    code->addCode(lineNum, stmt);
}

void Program::deleteCode(int lineNum)
{
    //根据行数删除某行代码
    code->deleteCode(lineNum);
}

Program::Program(Parser *parser, Status *status)
{
    this->parser = parser;
    code = new Code(parser);
    this->status = status;
    codeCounter = nullptr;
    bPoints.clear();
}

QString Program::extractStr()
{
    //提取出代码字符串，显示在代码显示框中
    return code->extractStr();
}

QString Program::getStrTree()
{
    //生成表达式树
    return code->getStrTree();
}

void Program::addBPoint(int lineNum)
{//添加断点
    if(lineNum <= 0 || lineNum > 1000000){
        //行号不在合法范围内，需要alert一下用户并无视这次操作
        //待完成
    }
    else if(this->code->findCode(lineNum) && bPoints.count(lineNum) == 0){
        bPoints.insert(lineNum);
    }
    else if(this->code->findCode(lineNum) == nullptr){
        //行号不存在，需要alert一下用户并无视这次操作
        //待完成
    }
}

void Program::deleteBPoint(int lineNum)
{//删除断点
    if(lineNum <= 0 || lineNum > 1000000){
        //行号不在合法范围内，需要alert一下用户并无视这次操作
        //待完成
    }
    else if(this->code->findCode(lineNum) && bPoints.count(lineNum) != 0){
        bPoints.erase(lineNum);
    }
    else if(this->code->findCode(lineNum) == nullptr){
        //行号不存在，需要alert一下用户并无视这次操作
        //待完成
    }
}

int Program::calculateExp(ExpNode *root)
{
    //输入表达式树根节点，计算表达式树的值
    if(root->type == exp_op){
        if(root->data == "+")return calculateExp(root->left) + calculateExp(root->right);
        else if(root->data == "-"){
            if(root->left)return calculateExp(root->left) - calculateExp(root->right);
            else return (- calculateExp(root->right));
        }
        else if(root->data == "*")return calculateExp(root->left) * calculateExp(root->right);
        else if(root->data == "/"){
            if(calculateExp(root->right) == 0){
                //除以0，报错
                //待完成
            }
            else return calculateExp(root->left) / calculateExp(root->right);
        }
        else if(root->data == "MOD"){
            if(calculateExp(root->left) > 0 && calculateExp(root->right) < 0){
                if(calculateExp(root->left) % calculateExp(root->right) != 0)return (calculateExp(root->left) % calculateExp(root->right) + calculateExp(root->right));
            }
            else return calculateExp(root->left) % calculateExp(root->right);
        }
        else if(root->data == "**"){
            int base = calculateExp(root->left);
            int exp = calculateExp(root->right);
            int result = 1;
            for(int i = 0; i < exp; i++)result *= base;
            return result;
        }
    }
    else if(root->type == exp_num)return root->data.toInt();
    else if(root->type == exp_var){
        if(status->isVarExist(root->data.toStdString()))return status->findVar(root->data.toStdString());
        else{
            //变量不存在，报错
            //待完成
        }
    }
}

QString Program::getBPointStr()
{//生成断点信息字符串，显示在断点框中
    QString result = "";
    for(auto it = bPoints.begin(); it != bPoints.end(); it++){
        result += QString::number(*it) + "\n";
    }
    return result;
}

void Program::exitDebug()
{//重置所有与调试有关的数据
    codeCounter = nullptr;
    bPoints.clear();
    if(status)status->clearVars();
}