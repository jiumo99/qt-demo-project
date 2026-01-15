#ifndef RECORD_H
#define RECORD_H
#include <QWidget>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>
#include "dbhelper.h"

namespace Ui {
class Record;
}

class Record : public QWidget
{
    Q_OBJECT
public:
    explicit Record(QWidget *parent = nullptr);
    ~Record();

    // ========== 改为公共函数 ==========
    void initModel();
    void refreshRecords(); // 新增公共刷新函数

public slots:  // 槽函数保持在这里
    void on_selectEdit_textChanged(const QString &arg1);
    void on_btnclearrecord_clicked();
    void on_btndatastatistics_clicked();
    void on_btncheckrecord_clicked();

    // 新增：公共方法，用于外部调用刷新
    void refreshOverdueRecords();

private:
    Ui::Record *ui;
    DBHelper *dbHelper;
    QSqlQueryModel *recordModel;
    QSortFilterProxyModel *proxyModel;
    void showOverdueAlert();
};

#endif // RECORD_H
