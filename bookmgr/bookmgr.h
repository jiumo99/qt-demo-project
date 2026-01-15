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
#include <QTextEdit> // 必须包含，否则QTextEdit识别失败
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
    QTextEdit *teDescription; // 图书描述
    QPushButton *btnOk;
    QPushButton *btnCancel;
};

class Bookmgr : public QWidget
{
    Q_OBJECT // 必须有这个宏才能使用信号槽

public:
    explicit Bookmgr(QWidget *parent = nullptr);
    ~Bookmgr();
    void setOperatorId(int operatorId) { m_operatorId = operatorId; }

    // ========== 新增：信号声明 ==========
signals:
    void borrowReturnSuccess(); // 借阅/归还成功的信号

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
    int m_operatorId; // 当前操作员ID
    void initModel();
    int getSelectedBookId();
};

#endif // BOOKMGR_H
