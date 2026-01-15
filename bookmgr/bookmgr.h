#ifndef BOOKMGR_H
#define BOOKMGR_H
#include <QWidget>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QTextEdit>
#include "dbhelper.h"

namespace Ui {
class Bookmgr;
}

class BookEditDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BookEditDialog(QWidget *parent = nullptr, bool isAdd = true, int book_id = 0);
    QStringList getBookInfo() const;
    bool isAddMode() const { return m_isAdd; }

private:
    bool m_isAdd;
    int m_book_id;
    // 新增：适配新表字段的控件
    QLineEdit *leBookId;
    QLineEdit *leIsbn;
    QLineEdit *leBookName;
    QLineEdit *leAuthor;
    QLineEdit *lePublisher;
    QDateEdit *dePublishDate;
    QLineEdit *leCategory;
    QLineEdit *leSubCategory;
    QLineEdit *leTags;
    QDoubleSpinBox *dsbPrice;
    QSpinBox *sbStock;
    QComboBox *cbBookStatus;
    QDoubleSpinBox *dsbOverdueFeeRate;
    QLineEdit *leCoverPath;
    QTextEdit *teDescription;
    QPushButton *btnOk;
    QPushButton *btnCancel;
};

class Bookmgr : public QWidget
{
    Q_OBJECT
public:
    explicit Bookmgr(QWidget *parent = nullptr);
    ~Bookmgr();
    // 新增：设置当前操作员ID（从登录用户获取）
    void setOperatorId(int operatorId) { m_operatorId = operatorId; }

private slots:
    void on_btnadd_clicked();
    void on_btnchange_clicked();
    void on_btndelete_clicked();
    void on_btnborrow_clicked();
    void on_btnreturn_clicked();
    void on_selectEdit_textChanged(const QString &arg1);

private:
    Ui::Bookmgr *ui;
    DBHelper *dbHelper;
    QSqlTableModel *bookModel;
    QSortFilterProxyModel *proxyModel;
    int m_operatorId; // 新增：当前操作员ID
    void initModel();
    int getSelectedBookId();
};
#endif // BOOKMGR_H
