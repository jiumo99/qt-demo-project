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
    // 新增：声明setOperatorId函数（关键修复点）
    void setOperatorId(int operatorId);

private slots:
    void on_btnbook_clicked();
    void on_btnuser_clicked();
    void on_btnrecord_clicked();

private:
    Ui::MainWindow *ui;
    Bookmgr *bookMgr;
    usermgr *userMgr;
    Record *recordMgr;
};

#endif // MAINWINDOW_H
