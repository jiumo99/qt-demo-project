#include "login.h"
#include "ui_login.h"
#include "dbhelper.h"
#include <QMessageBox>

Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    // 初始化数据库助手（全局唯一，可通过单例优化，此处简化）
    dbHelper = new DBHelper(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_btnLogin_clicked()
{
    QString username = ui->useredit->text().trimmed();
    QString password = ui->passwordedit->text().trimmed();

    // 空值校验
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名或密码不能为空！");
        return;
    }

    // 数据库验证
    if (dbHelper->isConnected() && dbHelper->verifyLogin(username, password)) {
        QMessageBox::information(this, "成功", "登录成功！");
        setResult(1);
        hide();
    } else {
        QMessageBox::critical(this, "失败", "用户名或密码错误！");
        ui->passwordedit->clear();
    }
}

void Login::on_btnexit_clicked()
{
    setResult(0);
    hide();
}
