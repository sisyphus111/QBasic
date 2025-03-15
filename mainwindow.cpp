#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUIExitDebugMode();

    waitInpVar = "";
    parser = new Parser();
    status = new Status();
    program = new Program(parser,status);
    debugMode = false;
    waitInpVal = false;

    connect(ui->btnDebugMode, &QPushButton::clicked, this, &MainWindow::setUIForDebugMode);
    connect(ui->btnExitDebugMode, &QPushButton::clicked, this, &MainWindow::setUIExitDebugMode);
    connect(ui->cmdLineEdit, &QLineEdit::returnPressed, this, &MainWindow::cmdLineEditFinished);
    connect(ui->btnLoadCode, &QPushButton::clicked, this, &MainWindow::loadCode);
    connect(ui->btnClearCode, &QPushButton::clicked, this, &MainWindow::initAll);
    connect(ui->btnRunCode, &QPushButton::clicked, this, &MainWindow::runCode);
    connect(ui->btnDebugResume, &QPushButton::clicked, this, &MainWindow::debugResume);
    connect(program, &Program::printSignal, this, &MainWindow::updateOutput);
    connect(program, &Program::waitInput, this, &MainWindow::startWaitInp);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::cmdLineEditFinished()
{//在代码输入框中按下回车
    //将输入的内容保存在cmd中
    QString cmd = ui->cmdLineEdit->text();
    ui->cmdLineEdit->setText("");

    if(!debugMode) {
        //非调试模式下的处理
        if(waitInpVal){
            cmd = cmd.last(cmd.size() - 1);//去掉"?"
            cmd = cmd.trimmed();
            //判断剩下的内容是不是一个数
            std::stringstream ss(cmd.toStdString());
            int val;
            ss >> val;
            if(QString::number(val).size() != cmd.size()){
                //输入的不是一个数，报错
                //待完成
            }
            else{
                //将输入的值赋给变量
                if(status->isVarExist(waitInpVar.toStdString()))status->modifyVar(waitInpVar.toStdString(), val);
                else status->addVar(waitInpVar.toStdString(), val);
                this->waitInpVal = false;
                this->waitInpVar = "";
                //发送信号，继续执行
                program->loop.exit();
            }
        }
        else {
            //先由Parser检验输入的种类：带行号代码/不带行号代码/指令/不合法输入
            InputType inpType = parser->tellInpType(cmd);
            if (inpType == INPUT_LINENUM_OUTOFRANGE) {
                //行号越界，报错
                ui->CodeDisplay->setText("INPLINENUMOUTOFRANGE");
            } else if (inpType == INP_INVALID) {
                //语句不合法，报错
                ui->CodeDisplay->setText("INPINVALID");
            } else if (inpType == UNINDEXED) {
                //不带行号代码，直接执行
                ui->CodeDisplay->setText("UNINDEXED");
            } else if (inpType == CMD) {
                //直接指令，执行
                //待完成
                if (cmd == "RUN") {
                    //运行代码
                    program->run();
                }
                else if (cmd == "LOAD") {
                    loadCode();//效果同点击“Load Code”按钮
                }
                else if (cmd == "LIST") {
                    //do nothing
                }
                else if (cmd == "CLEAR") {
                    initAll();//效果同点击清空按钮
                }
                else if (cmd == "HELP") {
                    //待完成
                }
                else if (cmd == "QUIT") {
                    //退出应用
                    //待完成

                }

            } else if (inpType == INDEXED) {
                //ui->CodeDisplay->setText("INDEXED");
                //带行号代码
                //通过splitIndexedInput函数，将输入分为行号和代码两部分
                std::pair<int, QString> p = parser->splitIndexedInput(cmd);
                int lineNum = p.first;//行号
                QString stmt = p.second;//代码
                if (stmt.trimmed().toStdString().empty())program->deleteCode(lineNum);//删除某行
                else program->addCode(lineNum, stmt);//添加一行代码
                //添加好之后，更新代码显示框和表达式树框
                ui->CodeDisplay->setText(program->extractStr());
                ui->treeDisplay->setText(program->getStrTree());
            }
        }
    }
    else{
        //调试模式下的处理
        //待完成
        if(cmd == "QUIT"){
            //quit指令，直接退出该应用
            //待完成
        }
        else if(cmd == "HELP"){
            //help指令，显示帮助信息
            //待完成
        }
        else{
            //需处理ADD,DELETE指令
            cmd = cmd.trimmed();//去掉首尾空格
            if(cmd.startsWith("ADD ")){
                //判断剩下的是不是行号
                cmd = cmd.last(cmd.size() - 4);//去掉"ADD "
                cmd = cmd.trimmed();
                std::stringstream ss(cmd.toStdString());
                int lineNum;
                if(ss >> lineNum && QString::number(lineNum).size() == cmd.size()){//确保剩下的内容是且仅是一个数（行号）
                    program->addBPoint(lineNum);
                }
                //更新断点显示框
                ui->breakPointsDisplay->setText(program->getBPointStr());
            }
            else if(cmd.startsWith("DELETE ")){
                //判断剩下的是不是行号
                cmd = cmd.last(cmd.size() - 7);//去掉"DELETE "
                cmd = cmd.trimmed();
                std::stringstream ss(cmd.toStdString());
                int lineNum;
                if(ss >> lineNum && QString::number(lineNum).size() == cmd.size()){//确保剩下的内容是且仅是一个数（行号）
                    program->deleteBPoint(lineNum);
                }
                //更新断点显示框
                ui->breakPointsDisplay->setText(program->getBPointStr());
            }
            else{
                //是不合法的输入，需要报错/无视
                //待完成

            }
        }
    }
}



void MainWindow::setUIForDebugMode()
{//进入调试模式

    debugMode = true;

    ui->btnClearCode->setVisible(false);
    ui->btnLoadCode->setVisible(false);
    ui->btnDebugMode->setVisible(false);

    ui->btnExitDebugMode->setVisible(true);
    ui->btnDebugResume->setVisible(true);

    ui->labelSyntaxTree->setVisible(false);
    ui->treeDisplay->setVisible(false);

    ui->labelMonitor->setVisible(true);
    ui->monitorDisplay->setVisible(true);
    ui->labelBreakPoints->setVisible(true);
    ui->breakPointsDisplay->setVisible(true);
}


void MainWindow::setUIExitDebugMode()
{//退出调试模式
    //将调试相关的数据结构/数据重置
    if(program)program->exitDebug();
    ui->monitorDisplay->setText("");
    ui->breakPointsDisplay->setText("");

    ui->btnClearCode->setVisible(true);
    ui->btnLoadCode->setVisible(true);
    ui->btnDebugMode->setVisible(true);

    ui->btnExitDebugMode->setVisible(false);
    ui->btnDebugResume->setVisible(false);

    ui->labelSyntaxTree->setVisible(true);
    ui->treeDisplay->setVisible(true);

    ui->labelMonitor->setVisible(false);
    ui->monitorDisplay->setVisible(false);
    ui->labelBreakPoints->setVisible(false);
    ui->breakPointsDisplay->setVisible(false);
}

//该函数仅在非调试模式下可被触发（由于按钮的设置）
void MainWindow::loadCode()//出现文件选择框，从文件加载代码（会清空原来的代码）
{
    //清空并重置一切
    this->initAll();
    //弹出文件选择框
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt)");
    if (fileName.isEmpty())return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))return;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        //将每一行代码加入到代码链表中
        //先判断是不是带行号代码
        InputType inpType = parser->tellInpType(line);
        if (inpType == INDEXED) {
            std::pair<int, QString> p = parser->splitIndexedInput(line);
            int lineNum = p.first;
            QString stmt = p.second;
            program->addCode(lineNum, stmt);
        }
    }
    //读取完毕，关闭文件
    file.close();
    //加载完毕，更新代码显示框和表达式树框
    ui->CodeDisplay->setText(program->extractStr());
    ui->treeDisplay->setText(program->getStrTree());
}


//该函数仅在非调试模式下可被触发（由于按钮的设置）
void MainWindow::initAll()//清空并重置一切，包括程序的运行状态、当前的代码、表达式树框、代码框、输出框内的内容
{
    delete this->status;
    delete this->program;

    debugMode = false;
    waitInpVal = false;
    waitInpVar = "";
    status = new Status();
    program = new Program(parser,status);

    connect(program, &Program::printSignal, this, &MainWindow::updateOutput);
    connect(program, &Program::waitInput, this, &MainWindow::startWaitInp);

    ui->CodeDisplay->setText("");
    ui->treeDisplay->setText("");
    ui->textBrowser->setText("");
    ui->monitorDisplay->setText("");
    ui->breakPointsDisplay->setText("");

}

void MainWindow::runCode()
{
    //需要先清理输出框
    ui->textBrowser->setText("");
    //清理之前的运行痕迹(即status中存储的键值对)
    status->clearVars();

    //“运行代码”按钮被点击时的处理函数（根据调试模式和非调试模式分别处理）
    if(debugMode){
        program->debugStart();//从头开始调试
        //更新变量框
        ui->monitorDisplay->setText(status->getVarStr());
    }
    else program->run();//直接运行
}

void MainWindow::debugResume()
{
    //"继续执行"按钮被点击时的处理函数
    program->debugResume();//（从某个断点处）继续执行
    //更新变量框
    ui->monitorDisplay->setText(status->getVarStr());
}

void MainWindow::updateOutput(QString str)
{
    //刷新输出框
    QString text = ui->textBrowser->toPlainText();
    text += str + "\n";
    ui->textBrowser->setText(text);
}

void MainWindow::startWaitInp(QString var)
{
    //开始等待输入变量的值
    waitInpVar = var;
    waitInpVal = true;
    //在命令输入框中显示一个'?'
    ui->cmdLineEdit->setText("?");
}