#pragma once
#include <QMainWindow>
#include "Status.h"
#include "Parser.h"
#include "Program.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Parser *parser;//语法句法分析器
    Status *status;//程序运行状态
    Program *program;//程序（包含代码）
    QString waitInpVar;//正在等待输入的变量名
    bool debugMode;//是否处于调试模式
    bool waitInpVal;//是否正在等待输入变量的值
    void setUIForDebugMode();
    void setUIExitDebugMode();
    void cmdLineEditFinished();//在输入框中按下回车
    void loadCode();//出现文件选择框，从文件加载代码（会清空原来的代码）
    void initAll();//清空并重置一切，包括程序的运行状态、当前的代码、表达式树框、代码框、输出框内的内容
    void runCode();//RUN按钮被点击的处理函数（注意调试模式与非调试模式的区别）
    void debugResume();//调试模式下的Resume功能
    void updateOutput(QString str);
    void startWaitInp(QString var);
};

