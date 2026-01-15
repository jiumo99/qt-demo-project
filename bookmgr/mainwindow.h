#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "bookmgr.h"
#include "usermgr.h"
#include "record.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // 切换到图书管理
    void on_btnbook_clicked();
    // 切换到读者管理
    void on_btnuser_clicked();
    // 切换到借阅管理
    void on_btnrecord_clicked();

private:
    Ui::MainWindow *ui;
    Bookmgr *bookMgr;
    usermgr *userMgr;
    Record *recordMgr;
};

#endif // MAINWINDOW_H
