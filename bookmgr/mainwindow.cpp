#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ========== 新增：创建按钮组并设置互斥 ==========
    btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->btnbook);
    btnGroup->addButton(ui->btnuser);
    btnGroup->addButton(ui->btnrecord);
    btnGroup->setExclusive(true); // 设置互斥，确保只有一个被选中

    // 设置默认选中图书管理按钮
    ui->btnbook->setChecked(true);

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

    // ========== 新增：连接信号槽，实现数据同步 ==========
    // 当借阅/归还成功时，刷新借阅管理界面
    connect(bookMgr, &Bookmgr::borrowReturnSuccess, this, [this]() {
        if (ui->stackedWidget->currentWidget() != recordMgr) {
            // 如果当前不是借阅管理界面，手动刷新模型
            recordMgr->refreshRecords();
        } else {
            // 如果是借阅管理界面，刷新当前显示（查看逾期记录）
            recordMgr->refreshOverdueRecords();
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setOperatorId(int operatorId)
{
    bookMgr->setOperatorId(operatorId);
}

void MainWindow::on_btnbook_clicked()
{
    ui->btnbook->setChecked(true);
    ui->stackedWidget->setCurrentWidget(bookMgr);
}

void MainWindow::on_btnuser_clicked()
{
    ui->btnuser->setChecked(true);
    ui->stackedWidget->setCurrentWidget(userMgr);
}

void MainWindow::on_btnrecord_clicked()
{
    ui->btnrecord->setChecked(true);
    ui->stackedWidget->setCurrentWidget(recordMgr);
}

void MainWindow::on_btnexit_clicked()
{
    if (QMessageBox::question(this, "确认退出", "确定要退出到登录界面吗？",
                              QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        // 直接关闭主窗口，程序会回到main.cpp中的循环，重新显示登录界面
        this->close();
    }
}
