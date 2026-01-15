// main.cpp
#include "mainwindow.h"
#include <QApplication>
#include "login.h"
#include "dbhelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 主循环：允许多次登录
    while (true) {
        Login dlg;
        int ret = dlg.exec();

        if (ret == 1) { // 登录成功
            MainWindow w;
            int operatorId = dlg.getLoggedInUserId();
            w.setOperatorId(operatorId);
            w.show();

            // 运行主窗口事件循环
            a.exec();

            // 当主窗口关闭时，会回到这里，重新开始循环显示登录界面
            continue;
        }
        else if (ret == 0) { // 用户点击退出登录界面的"退出"按钮
            break;
        }
    }

    return 0;
}
