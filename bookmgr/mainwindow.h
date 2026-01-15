#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QButtonGroup>  // 添加这行
#include <QCloseEvent>
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
    void setOperatorId(int operatorId);

private slots:
    void on_btnbook_clicked();
    void on_btnuser_clicked();
    void on_btnrecord_clicked();
    void on_btnexit_clicked();

private:
    Ui::MainWindow *ui;
    Bookmgr *bookMgr;
    usermgr *userMgr;
    Record *recordMgr;
    QButtonGroup *btnGroup;  // 新增：按钮组
};

#endif // MAINWINDOW_H
