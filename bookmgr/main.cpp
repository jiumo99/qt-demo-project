#include "mainwindow.h"
#include <QApplication>
#include "login.h"
#include "dbhelper.h"  // 包含DBHelper头文件

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 测试数据库连接
    DBHelper::getInstance()->openDatabase();

    Login dlg;
    int ret = dlg.exec();
    if(1 == ret)
    {
        MainWindow w;
        w.show();
        return a.exec();
    }
    if(0 == ret)
    {
        DBHelper::getInstance()->closeDatabase();  // 退出时关闭数据库
        exit(0);
        return 0;
    }
    return 0;
}
