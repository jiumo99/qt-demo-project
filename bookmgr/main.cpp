#include "mainwindow.h"
#include <QApplication>
#include "login.h"
#include "dbhelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login dlg;
    int ret = dlg.exec();
    if (ret == 1) // 登录成功
    {
        MainWindow w;
        // 获取登录用户ID并设置为操作员ID
        int operatorId = dlg.getLoggedInUserId();
        w.setOperatorId(operatorId);
        w.show();
        return a.exec();
    }
    else if (ret == 0) // 退出登录
    {
        exit(0);
    }
    return 0;
}
