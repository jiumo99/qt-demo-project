#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("图书与借阅管理系统");

    // 初始化三个功能界面
    bookMgr = new Bookmgr(this);
    userMgr = new usermgr(this);
    recordMgr = new Record(this);

    // 添加到StackedWidget
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
