#include "login.h"
#include "ui_login.h"
#include "dbhelper.h"
#include <QMessageBox>

Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    dbHelper = new DBHelper(this);
}

Login::~Login()
{
    delete ui;
}

// 新增：获取登录用户ID（用于传递给Bookmgr）
int Login::getLoggedInUserId() const
{
    QString username = ui->useredit->text().trimmed();
    return dbHelper->getUserIdByUsername(username);
}

void Login::on_btnLogin_clicked()
{
    QString username = ui->useredit->text().trimmed();
    QString password = ui->passwordedit->text().trimmed();
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "警告", "用户名或密码不能为空！");
        return;
    }
    if (dbHelper->isConnected() && dbHelper->verifyLogin(username, password)) {
        QMessageBox::information(this, "成功", "登录成功！");
        setResult(1);
        hide();
    } else {
        QMessageBox::critical(this, "失败", "用户名或密码错误，或账户已禁用！");
        ui->passwordedit->clear();
    }
}

void Login::on_btnexit_clicked()
{
    // 设置为拒绝状态，这样main.cpp中的循环会退出
    setResult(0);
    hide();
}
