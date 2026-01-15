#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 初始化三个子界面
    bookMgr = new Bookmgr();
    userMgr = new usermgr();
    recordMgr = new Record();

    // 将子界面添加到主窗口的stackedWidget
    ui->stackedWidget->addWidget(bookMgr);
    ui->stackedWidget->addWidget(userMgr);
    ui->stackedWidget->addWidget(recordMgr);

    // 默认显示图书管理界面
    ui->stackedWidget->setCurrentWidget(bookMgr);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 新增：实现setOperatorId函数（关键修复点）
void MainWindow::setOperatorId(int operatorId)
{
    bookMgr->setOperatorId(operatorId);
}

// 原有槽函数不变
void MainWindow::on_btnbook_clicked()
{
    ui->stackedWidget->setCurrentWidget(bookMgr);
}

void MainWindow::on_btnuser_clicked()
{
    ui->stackedWidget->setCurrentWidget(userMgr);
}

void MainWindow::on_btnrecord_clicked()
{
    ui->stackedWidget->setCurrentWidget(recordMgr);
}
