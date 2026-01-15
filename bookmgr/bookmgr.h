#ifndef BOOKMGR_H
#define BOOKMGR_H
#include <QWidget>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include "dbhelper.h"

namespace Ui {
class Bookmgr;
}

// 图书编辑对话框（添加/修改图书用）
class BookEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BookEditDialog(QWidget *parent = nullptr, bool isAdd = true, int bookid = 0);
    QStringList getBookInfo() const; // 返回图书信息：[bookid, type1, type2, type3, pic, name, press, cnt]
    bool isAddMode() const { return m_isAdd; }

private:
    bool m_isAdd;
    int m_bookid;
    QLineEdit *leBookid;
    QLineEdit *leType1;
    QLineEdit *leType2;
    QLineEdit *leType3;
    QLineEdit *lePic;
    QLineEdit *leName;
    QLineEdit *lePress;
    QLineEdit *leCnt;
    QPushButton *btnOk;
    QPushButton *btnCancel;
};

class Bookmgr : public QWidget
{
    Q_OBJECT
public:
    explicit Bookmgr(QWidget *parent = nullptr);
    ~Bookmgr();

private slots:
    // 按钮点击事件
    void on_btnadd_clicked();
    void on_btnchange_clicked();
    void on_btndelete_clicked();
    void on_btnborrow_clicked();
    void on_btnreturn_clicked();
    // 搜索框文本变化（筛选图书）
    void on_selectEdit_textChanged(const QString &arg1);

private:
    Ui::Bookmgr *ui;
    DBHelper *dbHelper;
    QSqlTableModel *bookModel; // 图书数据模型
    QSortFilterProxyModel *proxyModel; // 筛选代理模型

    // 初始化数据模型
    void initModel();
    // 获取选中行的图书ID
    int getSelectedBookId();
};

#endif // BOOKMGR_H
