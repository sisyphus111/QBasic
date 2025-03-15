#pragma once
#include "Parser.h"
#include <sstream>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <stack>
//判断输入内容的类型
InputType Parser::tellInpType(const QString input)
{
    QString inp = input.trimmed();//忽略开头结尾的空格
    std::string str = inp.toStdString();//转换为std::string

    //先判断是不是指令
    if(str == "RUN" || str == "LOAD" || str == "LIST" || str == "CLEAR" || str == "QUIT" || str == "HELP"){
        return CMD;
    }

    //判断开头是不是一个数
    std::stringstream ss(str);
    int lineNum;
    if( !(ss >> lineNum) ){//开头不是一个数，可能是不带行号代码/不合法内容
        //判断指令类型
        if(tellStmtType(QString::fromStdString(str)) == STMT_INVALID){//内容不合法
            return INP_INVALID;
        }
        else{//内容合法，则为不带行号代码
            return UNINDEXED;
        }
    }
    else {//开头是一个数(行号储存在lineNum中)可能是带行号代码（包括删除某行的指令）/不合法内容
        if(lineNum > 1000000 || lineNum <= 0)return INPUT_LINENUM_OUTOFRANGE;//行号不在合法范围内

        //注：即使已经输出了行号，ss.str()返回的仍然是完整的字符串，下面这行代码会手动去掉行号
        inp = QString::fromStdString(ss.str()).mid(QString::number(lineNum).size());//将剩余的内容转为QString

        //先判断是不是删除某行的指令
        if(inp.trimmed().toStdString().empty())return INDEXED;//删除某行的指令

        if((!inp.startsWith(' ')) || tellStmtType(inp) == STMT_INVALID){//行号和语句之间没有空格/内容不合法
            return INP_INVALID;
        }
        else if(tellStmtType(QString::fromStdString(str)) == STMT_LINENUM_OUTOFRANGE){//语句中（例如GOTO）出现了不在合法范围内的行号
            return INPUT_LINENUM_OUTOFRANGE;
        }
        else{
            return INDEXED;
        }
    }
}


//判断语句类型
StmtType Parser::tellStmtType(QString stmt)
{
    stmt = stmt.trimmed();//去掉开头和结尾的空格
    if(stmt.startsWith("REM "))return REM;
    else if(stmt == "END")return END;
    else if(stmt.startsWith("GOTO ")){
        //判断剩下的内容是不是行号
        stmt = stmt.remove("GOTO ");
        //再去掉首尾的空格
        stmt = stmt.trimmed();
        //判断剩下的是不是一个完整的合法行号
        std::string str = stmt.toStdString();
        std::stringstream ss(str);
        int lineNum;
        if(ss >> lineNum){//是一个数
            //行号应在合法范围内
            if(lineNum > 1000000 || lineNum <= 0)return STMT_LINENUM_OUTOFRANGE;
            //此时剩下部分应该为空

            QString remainingStr = QString::fromStdString(ss.str()).mid(QString::number(lineNum).size());
            if(remainingStr == "")return GOTO;

            else return STMT_INVALID;
        }
        else return STMT_INVALID;
    }
    else if(stmt.startsWith("PRINT ")){
        //判断剩下的内容是不是表达式
        stmt = stmt.remove("PRINT ");
        //再去掉首尾空格
        stmt = stmt.trimmed();
        //判断剩下的是不是一个合法表达式
        if(tellExpValidity(stmt.toStdString()))return PRINT;
        else return STMT_INVALID;
    }
    else if(stmt.startsWith("INPUT ")){
        //判断剩下的内容是不是变量名
        stmt = stmt.remove("INPUT ");
        //去掉首尾的空格
        stmt = stmt.trimmed();
        //判断剩下的是不是一个合法变量名
        if(tellVarValidity(stmt.toStdString()))return INPUT;
        else return STMT_INVALID;
    }
    else if(stmt.startsWith("LET")){
        //判断剩下的内容是不是：变量名=表达式
        stmt = stmt.remove("LET ");
        //去掉首尾的空格
        stmt = stmt.trimmed();
        //找到等号的位置
        int pos = stmt.indexOf('=');
        if(pos == -1)return STMT_INVALID;//没有等号
        //判断等号左右两边的内容是不是合法
        QString var = stmt.left(pos);
        QString exp = stmt.right(stmt.size() - pos - 1);
        if(tellVarValidity(var.trimmed().toStdString()) && tellExpValidity(exp.trimmed().toStdString()))return LET;
        else return STMT_INVALID;
    }
    else if(stmt.startsWith("IF ")){
        //判断剩下的内容是不是：表达式 THEN 行号
        stmt = stmt.remove("IF ");
        //去掉首尾的空格
        stmt = stmt.trimmed();
        //找到THEN的位置
        int pos = stmt.indexOf(" THEN ");
        if(pos == -1)return STMT_INVALID;//没有" THEN "
        //判断THEN左右两边的内容是不是合法
        QString exp = stmt.left(pos);
        exp = exp.trimmed();//IF和THEN中间的内容
        //应该有且仅有一个比较运算符
        bool contains[3] = {exp.contains('<'), exp.contains('='), exp.contains('>')};
        if(contains[0] + contains[1] + contains[2] != 1)return STMT_INVALID;
        size_t compPos = std::max({exp.indexOf('<'), exp.indexOf('='), exp.indexOf('>')});
        QString lExp = exp.left(compPos).trimmed();
        QString rExp = exp.right(exp.size() - compPos - 1).trimmed();

        QString line = stmt.right(stmt.size() - pos - 6);
        line = line.trimmed();
        if(tellExpValidity(lExp.toStdString()) && tellExpValidity(rExp.toStdString())){//表达式合法
            //判断剩下的是不是行号
            std::string str = line.toStdString();
            std::stringstream ss(str);
            int lineNum;
            if(ss >> lineNum){//是一个数
                //行号应在合法范围内
                if(lineNum > 1000000 || lineNum <= 0)return STMT_LINENUM_OUTOFRANGE;
                //此时剩下部分应该为空
                QString remainingStr = QString::fromStdString(ss.str()).mid(QString::number(lineNum).size());
                if(remainingStr == "")return IF;
                else return STMT_INVALID;
            }
            else return STMT_INVALID;
        }
        else return STMT_INVALID;
    }
    else return STMT_INVALID;
}

bool Parser::tellExpValidity(const std::string exp)
{//判断一个表达式是否合法
    //先去掉所有操作符两边的空格
    std::string expNoSpaces = removeSpacesAroundOperators(exp);
    //若表达式以'('开头，以')'结尾，则去掉这对括号；若开头是'-'则也可去掉
    while(expNoSpaces.front() == '(' && expNoSpaces.back() == ')' || expNoSpaces.front() == '-'){
        if(expNoSpaces.front() == '(' && expNoSpaces.back() == ')')expNoSpaces = expNoSpaces.substr(1, expNoSpaces.size() - 2);
        else if(expNoSpaces.front() == '-')expNoSpaces = expNoSpaces.substr(1, expNoSpaces.size() - 1);
    }
    if(expNoSpaces.empty() || exp.empty())return false;//表达式为空
    //判断表达式中是否有非法字符
    for(char ch : expNoSpaces){
        if(!std::isalnum(ch) && ch != '+' && ch != '-' && ch != '*' && ch != '/' && ch != '(' && ch != ')' && ch != ' ' && ch != 'M' && ch != 'O' && ch != 'D')return false;
    }
    //如果表达式是一个数，则返回true
    std::istringstream iss(expNoSpaces);
    int num;
    iss >> num;
    if(iss.eof())return true;
    //下面寻找括号位置，先检查是否匹配，再根据括号外的第一个运算符进行划分，然后递归处理
    std::vector<std::pair<int,int>> paras = findParasPairs(expNoSpaces);
    //若有不匹配的括号，则返回false
    if( !paras.empty() && paras[0].first == -1 && paras[0].second == -1)return false;
    //已经检查括号匹配或无括号
    if( !paras.empty() ){
        //有成对括号，则理论上必须有运算符在括号外
        int left = paras[0].first;
        int right = paras[0].second;
        //在左括号左边，右括号右边（直至字符串末尾或第二个左括号）寻找第一个运算符，然后递归处理
        int left2;
        if(paras.size() == 1)left2 = expNoSpaces.size();
        else {//left2的值为：第一个右括号右侧的第一个左括号的索引
            left2 = expNoSpaces.size();
            for(int i = 1; i < paras.size(); i++){
                if(paras[i].first > paras[0].second && paras[i].first < left2)left2 = paras[i].first;
            }
        }
        //从开头找到左括号
        for(int i = left - 1; i >= 0; i--){
            if(i >= 2 && expNoSpaces[i] == 'D' && expNoSpaces[i - 1] == 'O' && expNoSpaces[i - 2] == 'M'){
                //找到了MOD
                std::string leftExp = expNoSpaces.substr(0, i - 2);
                std::string rightExp = expNoSpaces.substr(i + 1, expNoSpaces.size() - i - 1);
                return tellExpValidity(leftExp) && tellExpValidity(rightExp);
            }
            else if(i >= 1 && expNoSpaces[i] == '*' && expNoSpaces[i-1] == '*'){
                //找到了**
                std::string leftExp = expNoSpaces.substr(0, i - 1);
                std::string rightExp = expNoSpaces.substr(i + 1, expNoSpaces.size() - i - 1);
                return tellExpValidity(leftExp) && tellExpValidity(rightExp);
            }
            else if(expNoSpaces[i] == '+' || expNoSpaces[i] == '-' || expNoSpaces[i] == '*' || expNoSpaces[i] == '/' ){
                //找到了运算符
                std::string leftExp = expNoSpaces.substr(0, i);
                std::string rightExp = expNoSpaces.substr(i + 1, expNoSpaces.size() - i - 1);
                return tellExpValidity(leftExp) && tellExpValidity(rightExp);
            }
        }
        //从右括号找到下一个左括号
        for(int i = right + 1; i < left2; i++){
            if(i + 2 < left2 && expNoSpaces[i] == 'M' && expNoSpaces[i+1] == 'O' && expNoSpaces[i+2] == 'D'){
                //找到了MOD
                std::string leftExp = expNoSpaces.substr(0, i);
                std::string rightExp = expNoSpaces.substr(i + 3, expNoSpaces.size() - i - 3);
                return tellExpValidity(leftExp) && tellExpValidity(rightExp);
            }
            else if(i + 1 < left2 && expNoSpaces[i] == '*' && expNoSpaces[i+1] == '*'){
                //找到了**
                std::string leftExp = expNoSpaces.substr(0, i);
                std::string rightExp = expNoSpaces.substr(i + 2, expNoSpaces.size() - i - 2);
                return tellExpValidity(leftExp) && tellExpValidity(rightExp);
            }
            else if(expNoSpaces[i] == '+' || expNoSpaces[i] == '-' || expNoSpaces[i] == '*' || expNoSpaces[i] == '/' ){
                //找到了运算符
                std::string leftExp = expNoSpaces.substr(0, i);
                std::string rightExp = expNoSpaces.substr(i + 1, expNoSpaces.size() - i - 1);
                return tellExpValidity(leftExp) && tellExpValidity(rightExp);
            }
        }
        //没找到，则返回false
        return false;
    }
    else{
        //没有括号
        //检查是否有运算符
        //若也无运算符，说明这就是个变量（需去掉首尾的空格）；若有运算符，则按照运算符分为两个字符串，递归处理
        //寻找运算符
        for(int i = 0; i < expNoSpaces.size(); i++){
            if(i + 2 < expNoSpaces.size() && expNoSpaces[i] == 'M' && expNoSpaces[i+1] == 'O' && expNoSpaces[i+2] == 'D'){
                //找到了MOD
                std::string leftExp = expNoSpaces.substr(0, i);
                std::string rightExp = expNoSpaces.substr(i + 3, expNoSpaces.size() - i - 3);
                return tellExpValidity(leftExp) && tellExpValidity(rightExp);
            }
            else if(i + 1 < expNoSpaces.size() && expNoSpaces[i] == '*' && expNoSpaces[i+1] == '*'){
                //找到了**
                std::string leftExp = expNoSpaces.substr(0, i);
                std::string rightExp = expNoSpaces.substr(i + 2, expNoSpaces.size() - i - 2);
                return tellExpValidity(leftExp) && tellExpValidity(rightExp);
            }
            else if(expNoSpaces[i] == '+' || expNoSpaces[i] == '-' || expNoSpaces[i] == '*' || expNoSpaces[i] == '/' ){
                //找到了运算符
                std::string leftExp = expNoSpaces.substr(0, i);
                std::string rightExp = expNoSpaces.substr(i + 1, expNoSpaces.size() - i - 1);
                return tellExpValidity(leftExp) && tellExpValidity(rightExp);
            }
        }
        //若无运算符，则将其视为变量
        //先去掉首尾的空格
        while(expNoSpaces.front() == ' ')expNoSpaces = expNoSpaces.substr(1, expNoSpaces.size() - 1);
        while(expNoSpaces.back() == ' ')expNoSpaces = expNoSpaces.substr(0, expNoSpaces.size() - 1);
        //判断是不是合法变量名
        return tellVarValidity(expNoSpaces);
    }
}

bool Parser::tellVarValidity(const std::string var)
{//判断一个变量名是否合法（已保证首尾无空格）
    // Reserved BASIC keywords
    static const std::unordered_set<std::string> keywords = {
            "LET", "REM", "PRINT", "INPUT", "GOTO", "IF", "END", "RUN", "LOAD", "LIST", "CLEAR", "QUIT", "HELP"
    };

    // Check if the variable name is a reserved keyword
    if (keywords.find(var) != keywords.end()) {
        return false;
    }

    // Check if the variable name is empty
    if (var.empty()) {
        return false;
    }

    // Check if the first character is a letter or underscore
    if (!std::isalpha(var[0]) && var[0] != '_') {
        return false;
    }

    // Check if the rest of the characters are letters, digits, or underscores
    for (size_t i = 1; i < var.size(); ++i) {
        if (!std::isalnum(var[i]) && var[i] != '_') {
            return false;
        }
    }

    return true;
}

//辅助函数，去掉表达式中，运算符两边的空格——已经测试
std::string Parser::removeSpacesAroundOperators(const std::string& exp)
{
    std::unordered_set<std::string> operators = {"+", "-", "*", "/", "MOD", "**", "(", ")" };
    std::string result;
    bool lastWasOperator = false;

    for (size_t i = 0; i < exp.size(); i++) {

        char ch = exp[i];
        if (lastWasOperator && std::isspace(ch)) {
            continue;
        }
        std::string op;
        if (i + 1 < exp.size() && exp[i] == '*' && exp[i + 1] == '*') {
            op = "**";
            i += 1;
        } else if (i + 2 < exp.size() && exp.substr(i, 3) == "MOD") {
            op = "MOD";
            i += 2;
        } else {
            op = std::string(1, ch);
        }

        if (operators.find(op) != operators.end()) {
            // Remove trailing spaces before the operator
            while (!result.empty() && std::isspace(result.back())) {
                result.pop_back();
            }
            result += op;
            lastWasOperator = true;
        } else {
            result += ch;
            lastWasOperator = false;
        }
    }
    return result;
}

//辅助函数，检查表达式中的括号匹配情况——已测试
std::vector<std::pair<int, int>> Parser::findParasPairs(const std::string& exp)
{
    std::stack<int> openParentheses;
    std::vector<std::pair<int, int>> parenthesesPairs;
    for (int i = 0; i < exp.size(); ++i) {
        if (exp[i] == '(') {
            openParentheses.push(i);
        } else if (exp[i] == ')') {
            if (!openParentheses.empty()) {
                int openPos = openParentheses.top();
                openParentheses.pop();
                parenthesesPairs.emplace_back(openPos, i);
            } else {
                // 未匹配的右括号
                parenthesesPairs.emplace_back(-1,-1);
            }
        }
    }
    // 检查未匹配的左括号
    while (!openParentheses.empty()) {
        int openPos = openParentheses.top();//不匹配左括号的位置，可删
        openParentheses.pop();
        parenthesesPairs.emplace_back(-1,-1);
    }
    // 根据左括号的索引位置排序
    std::sort(parenthesesPairs.begin(), parenthesesPairs.end());
    return parenthesesPairs;
}

Parser::Parser()
{//构造函数
    //do nothing
    //改进：可以将keywords，operators等std::unordered_set在此初始化
}

//将带行号代码的输入分为行号和代码两部分
std::pair<int, QString> Parser::splitIndexedInput(const QString inp)
{//调用该函数之前应该先确定输入inp是带行号的代码（即INDEXED类型）
    std::stringstream ss(inp.toStdString());
    int lineNum;
    ss >> lineNum;//提取行号
    QString stmt = QString::fromStdString(ss.str()).mid(QString::number(lineNum).size());//将剩下的代码部分放入QString类型的stmt中
    stmt = stmt.trimmed();//去掉首尾的空格
    return std::make_pair(lineNum, stmt);
}

//从Stmt类型的对象中提取出字符串
QString Parser::extractStmtStr(Stmt *stmt)
{
    if(stmt->type == REM)return "REM " + this->extractExpStr(stmt->exps[0]);
    else if(stmt->type == LET)return "LET " + this->extractExpStr(stmt->exps[0]) + " = " + this->extractExpStr(stmt->exps[1]);
    else if(stmt->type == PRINT)return "PRINT " + this->extractExpStr(stmt->exps[0]);
    else if(stmt->type == INPUT)return "INPUT " + this->extractExpStr(stmt->exps[0]);
    else if(stmt->type == GOTO)return "GOTO " + this->extractExpStr(stmt->exps[0]);
    else if(stmt->type == IF)return "IF " + this->extractExpStr(stmt->exps[0])
    + " " + this->extractExpStr(stmt->exps[1])
    + " " + this->extractExpStr(stmt->exps[2])
    + " THEN " + this->extractExpStr(stmt->exps[3]);
    else if(stmt->type == END)return "END";
    else return "INVALID";//理论上不会出现这种情况
}

//从Exp类型的对象中提取出字符串
QString Parser::extractExpStr(Exp *exp)
{
    if(exp->type == str || exp->type ==var || exp->type == op)return exp->root->data;//对于字符串、变量、操作符，只需要返回其根节点的数据即可
    else if(exp->type == num){
        //将表达式树转为一个字符串
        return expTreeToStr(exp->root);
    }
    else return "INVALID";//理论上不会出现此情况
}



Ops Parser::tellOperator(size_t i, const std::string &str)
{//判断是不是操作符，若是，则返回操作符类型
    if(i < str.size() - 2 && str.substr(i, 3) == "MOD")return MOD;
    else if(i < str.size() - 1 && str.substr(i, 2) == "**")return POW;
    else if(str[i] == '+')return ADD;
    else if(str[i] == '-')return SUB;
    else if(str[i] == '*')return MUL;
    else if(str[i] == '/')return DIV;
    else if(str[i] == '(')return LPARA;
    else if(str[i] == ')')return RPARA;
    else return INVALID;
}

int Parser::tellPrecedence(Ops op)
{//判断操作符优先级
    if(op == ADD || op == SUB)return 1;
    else if(op == MUL || op == DIV || op == MOD)return 2;
    else if(op == POW)return 3;
    else if(op == LPARA || op == RPARA)return 0;//括号特殊处理
    else return -1;
}

int Parser::tellPrecedence(QString op)
{//判断操作符优先级
    if(op == "+" || op == "-")return 1;
    else if(op == "*" || op == "/" || op == "MOD")return 2;
    else if(op == "**")return 3;
    else if(op == "(" || op == ")")return 0;//括号特殊处理
    else return -1;
}

int Parser::tellPrecedence(std::string op)
{//判断操作符优先级
    if(op == "+" || op == "-")return 1;
    else if(op == "*" || op == "/" || op == "MOD")return 2;
    else if(op == "**")return 3;
    else if(op == "(" || op == ")")return 0;//括号特殊处理
    else return -1;
}

ExpNode* Parser::strToExpTree(QString exp)//根据字符串构造一个表达式树（参数exp的格式理论上已经被tellExpValidity函数检测过了）
{
    std::stack<ExpNode*> nodes;//节点栈
    std::stack<std::string> ops;//操作符栈
    std::string str = exp.remove(' ').toStdString();//可以直接去掉所有空格（参数exp已经被tellExpValidity函数判断合法了）
    std::stack<ExpNode*> waitNodes;//等待左儿子归位的节点们
    size_t prev = 0;//使用前后两个索引，定位变量/数值所占的位置
    for(size_t i = 0; i < str.size();){
        Ops op = tellOperator(i, str);//判断i处是不是操作符

        if(op == INVALID){//不是操作符，就说明是数/变量的一部分
            i++;
            continue;
        }
        else{//是操作符
            if(op == LPARA){
                //左括号
                ops.push("(");//直接入栈即可
                prev = i + 1;
                i++;
            }
            else if(op == RPARA){
                //右括号
                while(ops.top() != "("){
                    //若有数字/变量在右括号前，则先将其入栈
                    if(prev < i){
                        ExpNode *node = new ExpNode(str.substr(prev, i - prev));//构造一个数/变量节点
                        nodes.push(node);
                        prev = i + 1;
                    }

                    //弹出操作符栈中的操作符，构造表达式树
                    Ops op = tellOperator(0, ops.top());
                    ops.pop();

                    //特殊情况：'-'在最前面表示负号
                    if(op == SUB && ops.top() == "("){
                        ExpNode* node = new ExpNode(exp_op, "-", nullptr, nodes.top());//构造负数节点
                        nodes.pop();//弹出正数节点

                        //若此时有节点在等待左儿子，则该节点就是它的左儿子
                        if(waitNodes.empty())nodes.push(node);//把负数节点压入栈
                        else{
                            while(!waitNodes.empty()){
                                //递归地将左儿子归位
                                ExpNode* p = waitNodes.top();
                                waitNodes.pop();
                                p->left = node;
                                node = p;
                            }
                            //此时node就是最终的根节点
                            nodes.push(node);
                        }

                        break;
                    }
                    //剩下的情况：正常处理二元运算符
                    ExpNode* node = new ExpNode(exp_op, getOpStr(op), nullptr, nodes.top());//右儿子必为栈顶结点，左儿子由于需要考虑优先级，先留空
                    nodes.pop();//弹出右儿子
                    if(tellPrecedence(op) > tellPrecedence(ops.top())){
                        //优先级够高了，可以直接构造
                        //左儿子为栈顶结点
                        node->left = nodes.top();
                        nodes.pop();

                        //若此时有节点在等待左儿子，则该节点就是它的左儿子
                        if(waitNodes.empty())nodes.push(node);
                        else{
                            while(!waitNodes.empty()){
                                //递归地将左儿子归位
                                ExpNode* p = waitNodes.top();
                                waitNodes.pop();
                                p->left = node;
                                node = p;
                            }
                            //此时node就是最终的根节点
                            nodes.push(node);
                        }
                    }

                    else{
                        //优先级不够高，需要等待左儿子
                        waitNodes.push(node);
                    }
                }
                //弹出'('
                ops.pop();
                prev = i + 1;//这时候prev指向一个右括号/运算符
                i++;
            }
            else{
                //二元操作符
                if(op == MOD){
                    //MOD
                    if(i >= 1 && exp[i - 1] != ')'){
                        ExpNode *node = new ExpNode(str.substr(prev, i - prev));//构造一个数/变量节点
                        nodes.push(node);
                    }
                    ops.push("MOD");
                    prev = i + 3;
                    i += 3;
                }
                else if(op == POW){
                    //**
                    if(i >= 1 && exp[i - 1] != ')'){
                        ExpNode *node = new ExpNode(str.substr(prev, i - prev));//构造一个数/变量节点
                        nodes.push(node);
                    }
                    ops.push("**");
                    prev = i + 2;
                    i += 2;
                }
                else{
                    //一位二元操作符
                    if(op == SUB){
                        //减号可能代表负数，特殊处理
                        if(i == 0 || exp[i - 1] == '('){
                            //负数
                            ops.push("-");
                            prev = i + 1;
                            i++;
                        }
                        else{
                            //减号
                            if(exp[i - 1] != ')'){
                                ExpNode *node = new ExpNode(str.substr(prev, i - prev));
                                nodes.push(node);
                            }
                            ops.push("-");
                            prev = i + 1;
                            i++;
                        }
                    }
                    else{
                        //普通一位二元操作符
                        if(exp[i - 1] != ')'){
                            ExpNode *node = new ExpNode(str.substr(prev, i - prev));
                            nodes.push(node);
                        }
                        ops.push(str.substr(i, 1));
                        prev = i + 1;
                        i++;
                    }
                }
            }
        }
    }
    //若最后是')'则prev = str.size()
    //处理最后一个数/变量
    if(prev < str.size()){
        ExpNode *node = new ExpNode(str.substr(prev, str.size() - prev));
        nodes.push(node);
    }
    //开始生成树
    while(!ops.empty()){
        //弹出操作符栈中的操作符，构造表达式树
        Ops op = tellOperator(0, ops.top());
        ops.pop();

        //特殊情况：'-'在最前面表示负号
        if(op == SUB && ops.empty()){
            ExpNode* node = new ExpNode(exp_op, "-", nullptr, nodes.top());//构造负数节点
            nodes.pop();//弹出正数节点

            //将负数压入栈
            //若此时有节点在等待左儿子，则该节点就是它的左儿子
            if(waitNodes.empty())nodes.push(node);//把负数节点压入栈
            else{
                while(!waitNodes.empty()){
                    //递归地将左儿子归位
                    ExpNode* p = waitNodes.top();
                    waitNodes.pop();
                    p->left = node;
                    node = p;
                }
                //此时node就是最终的根节点
                nodes.push(node);
            }

            break;
        }



        //剩下的情况：正常处理二元运算符
        ExpNode* node = new ExpNode(exp_op, getOpStr(op), nullptr, nodes.top());//右儿子必为栈顶结点，左儿子由于需要考虑优先级，先留空
        nodes.pop();//弹出右儿子
        if(ops.empty() || tellPrecedence(op) > tellPrecedence(ops.top())){
            //优先级够高了，可以直接构造
            //左儿子为栈顶结点
            node->left = nodes.top();
            nodes.pop();

            //若此时有节点在等待左儿子，则该节点就是它的左儿子
            if(waitNodes.empty())nodes.push(node);
            else{
                while(!waitNodes.empty()){
                    //递归地将左儿子归位
                    ExpNode* p = waitNodes.top();
                    waitNodes.pop();
                    p->left = node;
                    node = p;
                }
                //此时node就是最终的根节点
                nodes.push(node);
            }
        }

        else{
            //优先级不够高，需要等待左儿子
            waitNodes.push(node);
        }
    }
    //此时栈中只剩下一个节点，即根节点
    return nodes.top();
}


QString Parser::expTreeToStr(ExpNode *node)//将表达式树转为字符串
{
    if(node == nullptr)return "";
    if(node->left == nullptr && node->right == nullptr)return node->data;
    QString left = expTreeToStr(node->left);
    QString right = expTreeToStr(node->right);
    QString str = "";
    if(node->left != nullptr && node->left->type == exp_op && tellPrecedence(node->data) > tellPrecedence(node->left->data))left = "(" + left + ")";
    if(node->right != nullptr && node->right->type == exp_op && tellPrecedence(node->data) > tellPrecedence(node->right->data))right = "(" + right + ")";
    str = left + node->data + right;
    return str;
}

QString Parser::getOpStr(Ops op)
{
    if(op == ADD)return "+";
    else if(op == SUB)return "-";
    else if(op == MUL)return "*";
    else if(op == DIV)return "/";
    else if(op == MOD)return "MOD";
    else if(op == POW)return "**";
    else return "";
}

Stmt *Parser::strToStmt(QString inp)
{//根据字符串构造一个stmt类型的对象
    StmtType type = tellStmtType(inp);
    if(type == REM){
        //REM
        QString content = inp.remove("REM ").trimmed();//去掉"REM ",再去首尾空格
        ExpNode *root = new ExpNode(exp_str, content);
        Exp* exp = new Exp(str, root);
        return new Stmt(REM, exp);
    }
    else if(type == LET){
        //LET
        QString str = inp.remove("LET ");
        int pos = str.indexOf('=');
        QString varStr = str.left(pos).trimmed();
        QString expStr = str.right(str.size() - pos - 1).trimmed();
        Exp* exp1 = new Exp(var, new ExpNode(exp_var, varStr));
        Exp* exp2 = new Exp(num, strToExpTree(expStr));
        return new Stmt(LET, exp1, exp2);
    }
    else if(type == PRINT){
        //PRINT
        QString str = inp.remove("PRINT ").trimmed();
        Exp* exp = new Exp(num, strToExpTree(str));
        return new Stmt(PRINT, exp);
    }
    else if(type == INPUT){
        //INPUT
        QString str = inp.remove("INPUT ").trimmed();
        Exp* exp = new Exp(var, new ExpNode(exp_var, str));
        return new Stmt(INPUT, exp);
    }
    else if(type == GOTO){
        //GOTO
        QString str = inp.remove("GOTO ").trimmed();
        Exp* exp = new Exp(num, new ExpNode(exp_num, str));
        return new Stmt(GOTO, exp);
    }
    else if(type == IF){
        //IF
        QString strInp = inp.remove("IF ");
        int pos = strInp.indexOf(" THEN ");
        QString lstr = strInp.left(pos);
        QString rstr = strInp.right(strInp.size() - pos - 6).trimmed();//" THEN "右侧的部分，为行号
        //将lstr分为三部分：表达式1，表达式2（比较运算符），表达式3
        //寻找比较运算符的位置
        int opPos = std::max({lstr.indexOf("="), lstr.indexOf('<'), lstr.indexOf('>')});
        QString exp1 = lstr.left(opPos).trimmed();//左侧的部分，为表达式
        QString exp2 = lstr.mid(opPos, 1);//中间的部分，为比较运算符
        QString exp3 = lstr.right(lstr.size() - opPos - 1).trimmed();//右侧的部分，为表达式


        Exp* exp1_ = new Exp(num,strToExpTree(exp1));
        Exp* exp2_ = new Exp(str,new ExpNode(exp_str, exp2));//比较运算符当作字符串处理
        Exp* exp3_ = new Exp(num,strToExpTree(exp3));
        Exp* exp4_ = new Exp(num, new ExpNode(exp_num, rstr));
        return new Stmt(IF, exp1_, exp2_, exp3_, exp4_);
    }
    else if(type == END){
        //END
        return new Stmt(END);
    }
    else return nullptr;//语句不合法
}


QString Parser::stmtToStr(Stmt *stmt)
{//将Stmt类型转换为字符串
    if(stmt->type == REM){
        return "REM " + stmt->exps[0]->root->data;
    }
    else if(stmt->type == LET){
        return "LET " + stmt->exps[0]->root->data + " = " + expTreeToStr(stmt->exps[1]->root);
    }
    else if(stmt->type == PRINT){
        return "PRINT " + expTreeToStr(stmt->exps[0]->root);
    }
    else if(stmt->type == INPUT){
        return "INPUT " + stmt->exps[0]->root->data;
    }
    else if(stmt->type == GOTO){
        return "GOTO " + stmt->exps[0]->root->data;
    }
    else if(stmt->type == IF){
        return "IF "
        + expTreeToStr(stmt->exps[0]->root)
        + " " + stmt->exps[1]->root->data
        + " " + expTreeToStr(stmt->exps[2]->root)
        + " THEN " + stmt->exps[3]->root->data;
    }
    else if(stmt->type == END){
        return "END";
    }
    else return "INVALID";
}


QString Parser::extractStmtTreeStr(Stmt *stmt, int spaceNum)
{//提取出stmt的树形字符串,spaceNum表示每一层缩进的空格数量
    QString result = "";
    if(stmt == nullptr)return result;
    if(stmt->type == REM){
        result = QString(spaceNum, ' ') + stmt->exps[0]->root->data + "\n";
    }
    else if(stmt->type == LET){
        result = QString(spaceNum, ' ') + stmt->exps[0]->root->data + "\n"
                + QString(spaceNum, ' ') + extractExpTreeStr(stmt->exps[1]->root, spaceNum + 4) + "\n";
    }
    else if(stmt->type == PRINT){
        result = QString(spaceNum, ' ') + extractExpTreeStr(stmt->exps[0]->root, spaceNum + 4) + "\n";
    }
    else if(stmt->type == INPUT){
        result = QString(spaceNum, ' ') + stmt->exps[0]->root->data + "\n";
    }
    else if(stmt->type == GOTO){
        result += QString(spaceNum, ' ') + stmt->exps[0]->root->data + "\n";
    }
    else if(stmt->type == IF){
        result = QString(spaceNum, ' ') + extractExpTreeStr(stmt->exps[0]->root, spaceNum + 4) + "\n"
        + QString(spaceNum, ' ') + stmt->exps[1]->root->data + "\n"
        + QString(spaceNum, ' ') + extractExpTreeStr(stmt->exps[2]->root, spaceNum + 4) + "\n"
        + QString(spaceNum, ' ') + stmt->exps[3]->root->data + "\n";
    }
    else if(stmt->type == END){
        result = QString(spaceNum, ' ') + "END\n";
    }
    return result;
}

QString Parser::extractExpTreeStr(ExpNode *root, int spaceNum)
{//为表达式生成表达式树字符串.spaceNum为每行缩进的空格数
    if(root == nullptr)return "";
    if(root->left == nullptr && root->right == nullptr)
        return root->data;
    else if(root->left == nullptr && root->right != nullptr)
        return root->data + "\n"
        + QString(spaceNum, ' ') + extractExpTreeStr(root->right, spaceNum + 4);
    else if(root->left != nullptr && root->right == nullptr)
        return root->data + "\n"
        + QString(spaceNum, ' ') + extractExpTreeStr(root->left, spaceNum + 4);
    else
        return root->data + "\n"
        + QString(spaceNum, ' ') + extractExpTreeStr(root->left, spaceNum + 4) + "\n"
        + QString(spaceNum, ' ') + extractExpTreeStr(root->right, spaceNum + 4);
}