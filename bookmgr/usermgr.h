#ifndef USERMGR_H
#define USERMGR_H
#include <QWidget>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QTextStream>
#include "dbhelper.h"

namespace Ui {
class usermgr;
}

class usermgr : public QWidget
{
    Q_OBJECT
public:
    explicit usermgr(QWidget *parent = nullptr);
    ~usermgr();

private slots:
    // 搜索用户
    void on_selectuserEdit_textChanged(const QString &arg1);
    // 删除用户
    void on_btndelectuser_clicked(); // 注意：UI控件名是btndelectuser（可能笔误，按原UI保留）
    // 导入用户（CSV格式）
    void on_btnimportuser_clicked();

private:
    Ui::usermgr *ui;
    DBHelper *dbHelper;
    QSqlTableModel *userModel;
    QSortFilterProxyModel *proxyModel;

    void initModel();
    int getSelectedUserId();
};

#endif // USERMGR_H
