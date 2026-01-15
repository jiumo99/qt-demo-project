#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

namespace Ui {
class Login;
}

class DBHelper; // 前置声明

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    int getLoggedInUserId() const;

private slots:
    void on_btnLogin_clicked();
    void on_btnexit_clicked();

private:
    Ui::Login *ui;
    DBHelper *dbHelper;
};

#endif // LOGIN_H
