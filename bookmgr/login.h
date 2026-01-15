#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "dbhelper.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void on_btnLogin_clicked();

    void on_btnexit_clicked();

private:
    Ui::Login *ui;
    DBHelper *dbHelper;
};

#endif // LOGIN_H
