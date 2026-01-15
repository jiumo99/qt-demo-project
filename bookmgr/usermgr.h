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
    void on_selectuserEdit_textChanged(const QString &arg1);
    void on_btndelectuser_clicked();
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
