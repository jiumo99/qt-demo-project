#include "mainwindow.h"
#include <QApplication>
#include "login.h"
#include "dbhelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login dlg; // 登录对话框已内部创建 DBHelper，无需额外创建
    int ret = dlg.exec();

    if (ret == 1) // 登录成功
    {
        MainWindow w;
        w.show();
        return a.exec();
    }
    else if (ret == 0) // 退出登录
    {
        exit(0);
    }
    return 0;
}
